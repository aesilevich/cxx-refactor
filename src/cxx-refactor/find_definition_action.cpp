// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file find_definition_action.cpp
/// Contains implementation of the find_definition_action class.

#include "find_definition_action.hpp"
#include <boost/program_options.hpp>


namespace po = boost::program_options;


boost::program_options::options_description find_definition_action::opts() const {
    po::options_description desc{"find-definition arguments"};
    desc.add_options()
        ("position", po::value<std::string>()->required(), "Position of symbol in source code");
    return desc;
}


void find_definition_action::perform(const cm::src::source_code_model & cm,
                                     const boost::program_options::variables_map & opts) const {
    // parsing source position
    auto pos_str = opts["position"].as<std::string>();
    auto pos_desc = cm::src::source_file_position_desc::from_string(pos_str);

    // looking source file with specified path
    auto src = cm.find_source(pos_desc.path(), true);
    if (src == nullptr) {
        std::ostringstream msg;
        msg << "can't find source file: '" << pos_desc.path() << "' in code model";
        throw std::runtime_error{msg.str()};
    }

    cm::src::source_file_position pos{src->cm_src(), pos_desc.pos()};

    // looking for AST node located at specified position
    auto node = cm.find_node_at_pos(pos);
    if (!node) {
        std::ostringstream msg;
        msg << "can't find AST node located at source position " << pos;
        throw std::runtime_error{msg.str()};
    }

    // std::cout << "FOUND NODE: " << node->class_name() << " "
    //           << node->source_range() << std::endl;

    // checking if node is an identifier
    auto ident = dynamic_cast<const cm::src::identifier*>(node);
    if (!ident) {
        std::ostringstream msg;
        msg << "can't find symbol at source position " << pos << ": "
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

    auto ctx_ent = dynamic_cast<const cm::context_entity*>(ent);
    if (!ctx_ent) {

        std::ostringstream msg;
        msg << "code model entity for AST node located at specified position is not an "
            << "user defined entity";
        throw std::runtime_error{msg.str()};
    }

    auto loc = ctx_ent->loc();
    if (!loc.is_valid()) {
        std::ostringstream msg;
        msg << "can't find location of code model entity associated with AST node located "
            << "at specified position";
        throw std::runtime_error{msg.str()};
    }

    auto named_ent = dynamic_cast<const cm::named_entity*>(ent);
    std::string symbol_name = named_ent ? named_ent->name() : "<unnamed>";
    std::cout << "Symbol " << symbol_name << " is defined at: " << loc << std::endl;
}
