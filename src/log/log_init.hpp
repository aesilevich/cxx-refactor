// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file log_init.hpp
/// Contains declarations of common log initialization functions

#pragma once

#include <filesystem>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/named_scope.hpp>
#include <boost/log/trivial.hpp>


/// Initializes log sinks and formatting
void log_init(bool log_console, const std::filesystem::path & log_file = {});

/// Initializes log and configures log from porgram options
void log_init(const boost::program_options::variables_map & vars,
              bool log_console = true,
              const std::filesystem::path & def_log_file = {});

/// Creates options description for log configuration options
boost::program_options::options_description log_options();

/// Sets up formatting for sink
template <typename Sink>
void setup_sink_formatting(Sink & sink) {
    auto fmt = [](const boost::log::record_view & rec, boost::log::formatting_ostream & str) {
        str << "[" << rec[boost::log::trivial::severity] << "] ";

        auto cat = boost::log::extract_or_default<std::string>("Category", rec, std::string{});
        auto subcat = boost::log::extract_or_default<std::string>("Subcategory", rec, std::string{});

        if (subcat.empty()) {
            str << "[" << cat << "]";
        } else {
            str << "[" << cat << "/" << subcat << "]";
        }

        str << " " << rec[boost::log::expressions::smessage];
    };

    sink.set_formatter(fmt);
}
