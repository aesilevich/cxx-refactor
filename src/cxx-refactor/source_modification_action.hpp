// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_modification_action.hpp
/// Contains definition of the source_modification_action class.

#pragma once

#include "multi_source_modifications.hpp"
#include "refactor_action.hpp"


/// Source modification refactor action
class source_modification_action: public refactor_action {
public:
    /// Constructs and returns options description for this action
    boost::program_options::options_description opts() const override;

    /// Performs action
    void perform(const cm::src::source_code_model & cm,
                 const boost::program_options::variables_map & opts) const override;

private:
    /// Performs action. Returns sources modifications
    virtual multi_source_modifications
    perform_mod(const cm::src::source_code_model & cm,
                const cm::src::source_file * src_file,
                const cm::src::source_position & pos) const = 0;
};
