// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file multi_source_modifications.hpp
/// Contains definition of the multi_source_modifications class.

#pragma once

#include "single_source_modifications.hpp"
#include <filesystem>


/// Modifications in multiple source files
class multi_source_modifications {
public:
    /// Constructs object
    explicit multi_source_modifications() = default;

    /// Adds modification for specified source file
    void add(const std::filesystem::path & src_path, const source_modification & mod) {
        mods_[src_path].add(mod);
    }

    /// Returns const reference to map of all modifications
    auto & mods() const { return mods_; }

private:
    /// Map of modifications for all source files
    std::map<std::filesystem::path, single_source_modifications> mods_;
};
