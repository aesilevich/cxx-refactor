// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file single_source_modifications.hpp
/// Contains definition of the single_source_modifications class.

#pragma once

#include "source_modification.hpp"
#include <map>


/// Represents list of modifications in a single source file
class single_source_modifications {
public:
    /// Constructs empty list of modifications
    explicit single_source_modifications() = default;

    /// Adds modification. Checks for overlapping with existing modifications
    void add(const source_modification & mod) {
        auto it = mods_.lower_bound(mod.range().start());
        if (it != mods_.end()) {
            // checking for modification range intersection
            if (mod.range().end() > it->first) {
                std::ostringstream msg;
                msg << "intersecting modifications are not supported";
                throw std::runtime_error{msg.str()};
            }
        }

        mods_.emplace(mod.range().start(), mod);
    }

    /// Returns range of source modifications ordered by start positions
    auto mods() const {
        auto fn = [](const auto & pair) -> const source_modification & { return pair.second; };
        return mods_ | std::ranges::views::transform(fn);
    }

private:
    /// Map of modifications
    std::map<cm::src::source_position, source_modification> mods_;
};
