// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file refactor_action.hpp
/// Contains definition of the refactor_action class

#pragma once

#include <string>
#include <cm/src/cmsrc.hpp>
#include <boost/program_options.hpp>


class refactor_action {
public:
    /// Virtual destructor
    virtual ~refactor_action() = default;

    /// Returns action name
    virtual std::string name() const = 0;

    /// Constructs and returns options description for this action
    virtual boost::program_options::options_description opts() const = 0;

    /// Performs action
    virtual void perform(const cm::src::source_code_model & cm,
                         const boost::program_options::variables_map & opts) const = 0;
};
