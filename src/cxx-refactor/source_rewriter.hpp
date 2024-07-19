// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_rewriter.hpp
/// Contains definition of the source_rewriter class.

#pragma once

#include "single_source_modifications.hpp"
#include "source_modification.hpp"
#include <filesystem>
#include <map>


/// Source rewriter
class source_rewriter {
public:
    explicit source_rewriter() = default;

    /// Rewrites single source from input stream to output stream
    void rewrite(const single_source_modifications & mods,
                 std::istream & istr,
                 std::ostream & ostr);

    /// Rewrites single source from file located at specified path to output stream
    void rewrite(const single_source_modifications & mods,
                 const std::filesystem::path & input,
                 std::ostream & ostr);
};
