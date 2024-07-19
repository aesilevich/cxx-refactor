// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_parameter_remove_action.cpp
/// Contains implementation of the template_parameter_remove_action class.

#include "pch.hpp"
#include "template_parameter_remove_action.hpp"


/// Creates modification for removing template argument
static source_modification
remove_template_argument(const cm::src::template_argument_spec * arg,
                         const cm::src::template_argument_spec * prev_arg,
                         const cm::src::template_argument_spec * next_arg) {
    auto remove_range = arg->source_range().range();

    // adjusting beginning of remove range if parameter is not the first parameter
    if (prev_arg != nullptr) {
        auto prev_end = prev_arg->source_range().range().end();
        assert(prev_end <= remove_range.start() && "invalid prev parameter range end");
        remove_range.set_start(prev_arg->source_range().range().end());
    }

    // adjusting end of remove range if parameter is not the last parameter
    if (next_arg != nullptr) {
        auto next_start = next_arg->source_range().range().start();
        assert(next_start >= remove_range.end() && "invalid next parameter range start");
        remove_range.set_end(next_arg->source_range().range().start());
    }

    return source_modification{remove_range, {}};
}


multi_source_modifications
template_parameter_remove_action::perform_mod(const cm::src::source_code_model & cm,
                                              const cm::src::source_file * src_file,
                                              const cm::src::source_position & pos) const {
    // looking for AST node located at specified position
    cm::src::source_file_position src_pos{src_file->cm_src(), pos};
    auto node = cm.find_node_at_pos(src_pos);
    if (!node) {
        std::ostringstream msg;
        msg << "can't find AST node located at source position " << src_pos;
        throw std::runtime_error{msg.str()};
    }

    // checking if node is an identifier
    auto ident = dynamic_cast<const cm::src::identifier*>(node);
    if (!ident) {
        std::ostringstream msg;
        msg << "can't find symbol at source position " << src_pos << ": "
            << "AST node located at specified position is not an identifier: "
            << node->class_name() << " " << node->source_range();
        throw std::runtime_error{msg.str()};
    }

    // looking for code model entity associated with identifier
    auto ent = ident->entity();
    if (ent == nullptr) {
        std::ostringstream msg;
        msg << "can't find code model entity associated with identifier AST node "
            << "located at specified position";
        throw std::runtime_error{msg.str()};
    }

    // checking that entity is a template parameter
    auto par = dynamic_cast<const cm::template_parameter*>(ent);
    if (par == nullptr) {
        std::ostringstream msg;
        msg << "code model entity located at specified position is not a template parameter: ";
        ent->print_desc(msg);
        throw std::runtime_error{msg.str()};
    }

    // getting template
    auto templ = par->templ();
    assert(templ != nullptr && "no template associated with template parameter");

    // // getting index of template parameter
    auto params = templ->template_params();
    auto param_it = std::ranges::find(params, par);
    assert(param_it != std::ranges::end(params) && "can't find template parameter");
    auto params_size = std::ranges::distance(params);
    auto param_idx = std::distance(std::ranges::begin(params), param_it);
    assert(param_idx < params_size && "invalid template parameter index");
    // std::cerr << "PARAMETER IDX: " << param_idx << std::endl;

    multi_source_modifications mods;

    // iterating over all template substitutions and removing argument
    for (auto && use : templ->uses()) {
        if (auto subst = dynamic_cast<const cm::template_substitution*>(use)) {
            // std::cerr << "TEMPLATE SUBSTITUTION: ";
            // subst->print_desc(std::cerr);
            // std::cerr << std::endl;

            for (auto subst_spec : subst->uses<cm::src::template_substitution_spec>()) {
                // std::cerr << "TEMPLATE SUBSTITUTION SPEC USE: " << node->class_name() << std::endl;

                // seatching for template argument spec and previous/next arguments
                auto args = subst_spec->arguments();
                assert(param_idx < std::ranges::size(args) && "template parameters inconsistency");
                auto arg_it = std::ranges::begin(args);
                std::ranges::advance(arg_it, param_idx);

                const cm::src::template_argument_spec * prev_arg = nullptr;
                const cm::src::template_argument_spec * next_arg = nullptr;

                if (param_idx != 0) {
                    // TODO: bug in std::prev in libc++
                    auto prev_arg_it = arg_it;
                    --prev_arg_it;
                    prev_arg = *prev_arg_it;
                }

                if (param_idx != params_size - 1) {
                    next_arg = *(std::next(arg_it));
                }

                auto mod = remove_template_argument(*arg_it, prev_arg, next_arg);
                mods.add(src_file->cm_src()->path(), mod);
            }
        } else if (auto ent = dynamic_cast<const cm::entity*>(use)) {
            std::cerr << "ENTITY USE: ";
            ent->print_desc(std::cerr);
            std::cerr << std::endl;
        } else if (auto node = dynamic_cast<const cm::src::ast_node*>(use)) {
            // std::cerr << "NODE USE: "
            //           << node->class_name()
            //           << ", " << node->source_range()
            //           << std::endl;
        } else {
            std::cerr << "UNKNOWN USE" << std::endl;
        }
    }


    // iterating over all parameter uses
    for (auto && use : par->uses()) {
        auto node = dynamic_cast<const cm::src::ast_node *>(use);
        if (!node) {
            continue;
        }

        if (auto par_decl = dynamic_cast<const cm::src::template_parameter_decl*>(node)) {
            // template parameter declaration for template class itself
            // or for outline members declarations

            // std::cerr << "TEMPLATE PARAMETER DECL USE: "
            //           << node->class_name() << ": "
            //           << node->source_range() << std::endl;

            // getting declarations of previous and next parameters
            auto prev_par_decl = par_decl->prev();
            auto next_par_decl = par_decl->next();

            auto remove_range = par_decl->source_range().range();

            // adjusting beginning of remove range if parameter is not the first parameter
            if (prev_par_decl != nullptr) {
                auto prev_end = prev_par_decl->source_range().range().end();
                assert(prev_end < remove_range.start() && "invalid prev parameter range end");
                remove_range.set_start(prev_par_decl->source_range().range().end());
            }

            // adjusting end of remove range if parameter is not the last parameter
            if (next_par_decl != nullptr) {
                auto next_start = next_par_decl->source_range().range().start();
                assert(next_start > remove_range.end() && "invalid next parameter range start");
                remove_range.set_end(next_par_decl->source_range().range().start());
            }

            // adding remove modification
            mods.add(src_file->cm_src()->path(), source_modification{remove_range, {}});

            continue;
        }
        
        if (auto spec = dynamic_cast<const cm::src::template_param_type_spec*>(node)) {
            // checking for special case when parameter is used for referencing template record
            // itself inside template definition
            if (auto targ_spec =
                dynamic_cast<const cm::src::template_argument_spec*>(spec->parent())) {
                
                auto subst_spec = targ_spec->parent();
                if (dynamic_cast<const cm::src::template_record_type_spec*>(subst_spec) ||
                    dynamic_cast<const cm::src::template_record_scope_spec*>(subst_spec)) {

                    // removing template argument from template substitution
                    auto mod = remove_template_argument(targ_spec,
                                                        targ_spec->find_prev(),
                                                        targ_spec->find_next());
                    mods.add(src_file->cm_src()->path(), mod);

                    continue;
                }
            }

            // template parameter is used as another type specification, replacing it with ???

            // std::cerr << "TEMPLATE PARAMETER TYPE SPEC USE: "
            //         << node->class_name() << ": "
            //         << node->source_range() << std::endl;

            auto sz = spec->name()->string().size();
            std::string replace_str;
            for (size_t i = 0; i < sz; ++i) {
                replace_str.push_back('?');
            }

            mods.add(src_file->cm_src()->path(),
                    source_modification{spec->source_range().range(), replace_str});
            continue;
        }

        std::cerr << "UNKNOWN TEMPLATE PARAMETER USE:"
                  << node->class_name() << ": "
                  << node->source_range() << std::endl;
    }

    return mods;
}
