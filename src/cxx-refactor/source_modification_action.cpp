// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_modification_action.cpp
/// Contains implementation of the source_modification_action class.

#include "source_modification_action.hpp"
#include "source_rewriter.hpp"
#include <fstream>


namespace po = boost::program_options;


boost::program_options::options_description source_modification_action::opts() const {
    po::options_description desc{"Source modification arguments"};
    desc.add_options()
        ("position", po::value<std::string>()->required(), "Position of symbol in source code");
    return desc;
}


void source_modification_action::perform(const cm::src::source_code_model & cm,
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

    // performing modification refactor action
    auto mods = perform_mod(cm, src, pos.pos());
    assert(!mods.mods().empty() && "refactor action returned empty set of modifications");

    // printing output source

    assert(mods.mods().size() == 1 && "multi source modifications are not supported yet");
    auto & src_mods = mods.mods().begin()->second;

    auto src_path_str = src->cm_src()->path().string();
    std::ifstream istr{src_path_str};
    if (!istr.is_open()) {
        std::ostringstream msg;
        msg << "can't open input source file for reading: " << src_path_str;
        throw std::runtime_error(msg.str());
    }

    source_rewriter rw;
    rw.rewrite(src_mods, istr, std::cout);
}
