// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_modification.hpp
/// Contains definition of the source_modification class.

#pragma once

#include <cm/src/cmsrc.hpp>


/// Represents modification in source code
class source_modification {
public:
    /// Constructs modification
    explicit source_modification(const cm::src::source_range & r,
                                 const std::string & insert_s):
        range_{r}, insert_str_{insert_s} {}

    /// Returns modification range
    const auto & range() const { return range_; }

    /// Returns reference to modification range
    auto & range() { return range_; }

    /// Returns insert string for modification
    const auto & insert_string() const { return insert_str_; }

    /// Sets insert string for modification
    void set_insert_string(const std::string & s) { insert_str_ = s; }

private:
    cm::src::source_range range_;           ///< Modification range
    std::string insert_str_;                ///< Insert string for modification
};
