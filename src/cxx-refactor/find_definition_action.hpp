// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file find_definition_action.hpp
/// Contains definition of the find_definition_action class.

#pragma once


#include "refactor_action.hpp"


/// Find definition action
class find_definition_action: public refactor_action {
public:
    /// Constructs action
    explicit find_definition_action() = default;

    /// Returns actions name
    std::string name() const override { return "find-definition"; }

    /// Constructs and returns options description for this action
    boost::program_options::options_description opts() const override;

    /// Performs action
    void perform(const cm::src::source_code_model & cm,
                 const boost::program_options::variables_map & opts) const override;
};
