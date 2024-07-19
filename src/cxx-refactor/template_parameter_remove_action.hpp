// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_parameter_remove_action.hpp
/// Contains definition of the template_parameter_remove_action class.

#pragma once

#include "source_modification_action.hpp"


/// Template parameter remove action
class template_parameter_remove_action: public source_modification_action {
public:
    /// Returns action name
    std::string name() const override { return "template-parameter-remove"; }

private:
    /// Performs action
    multi_source_modifications
    perform_mod(const cm::src::source_code_model & cm,
                const cm::src::source_file * src_file,
                const cm::src::source_position & pos) const override;
};
