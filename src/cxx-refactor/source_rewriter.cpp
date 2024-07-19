// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_rewriter.cpp
/// Contains implementation of the source_rewriter class.

#include "pch.hpp"
#include "source_rewriter.hpp"
#include <fstream>


/// Input stream with position
class istream_with_position {
public:
    explicit istream_with_position(std::istream & s):
    str_{s}, ch_{0} {
        pos_.set_line(1);
        pos_.set_column(1);
        str_.read(&ch_, 1);
    }

    /// Reads next character from input stream and increases current position.
    void read_next() {
        if (ch_ == '\n') {
            pos_.set_line(pos_.line() + 1);
            pos_.set_column(1);
        } else {
            pos_.set_column(pos_.column() + 1);
        }

        str_.read(&ch_, 1);
    }

    /// Returns current character
    char ch() const { return ch_; }

    /// Returns current position
    const auto & pos() { return pos_; }

    /// Returns true if EOF is reached
    bool eof() const { return str_ ? false : true; }

private:
    std::istream & str_;                    ///< Reference to input stream
    char ch_;                               ///< Current character
    cm::src::source_position pos_;          ///< Current position
};


void source_rewriter::rewrite(const single_source_modifications & smods,
                              std::istream & str,
                              std::ostream & ostr) {

    istream_with_position istr{str};

    auto mods = smods.mods();
    auto mod_it = std::ranges::begin(mods);

    // processing all data from input source
    while (!istr.eof()) {
        while (mod_it != mods.end()) {
            auto mod_start = (*mod_it).range().start();
            auto mod_end = (*mod_it).range().end();

            if (istr.pos() == mod_start) {
                // skipping source characters until the end of modification range
                while (istr.pos() != mod_end) {
                    istr.read_next();

                    // checking for missing modification end location in source code
                    if (istr.eof() || istr.pos() > mod_end) {
                        std::ostringstream msg;
                        msg << "can't find modification end location in source code: ("
                            << mod_end.line() << ", " << mod_end.column() << ")";
                        throw std::runtime_error{msg.str()};
                    }
                }

                // writing modification string to output
                ostr << (*mod_it).insert_string();

                // moving to the next modification
                ++mod_it;
            } else {
                // checking for misssing modification start location in source code
                if (istr.pos() > mod_start) {
                    std::ostringstream msg;
                    msg << "can't find modification start location in source code: ("
                        << mod_start.line() << ", " << mod_start.column() << ")";
                    throw std::runtime_error{msg.str()};
                }

                // exiting from modifications loop to write current character
                break;
            }
        }

        // writing current character to output stream
        auto c = istr.ch();
        ostr.write(&c, 1);

        // reading next character
        istr.read_next();
    }

    // checking that we found all modification locations
    if (mod_it != std::ranges::end(mods)) {
        std::ostringstream msg;
        msg << "can't find modification start location in source code: ("
            << (*mod_it).range().start().line() << ", "
            << (*mod_it).range().start().column() << ")";
        throw std::runtime_error{msg.str()};
    }
}


void source_rewriter::rewrite(const single_source_modifications & smods,
                              const std::filesystem::path & input,
                              std::ostream & ostr) {
    // opening input source file
    std::ifstream file{input.string(), std::ios::binary};
    if (!file.is_open()) {
        std::ostringstream msg;
        msg << "can't open input file " << input << " for reading";
        throw std::runtime_error{msg.str()};
    }

    file.unsetf(std::ios::skipws);

    rewrite(smods, file, ostr);
}
