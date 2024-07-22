// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file log_init.cpp
/// Contains implementation of common log initialization functions.

#include "log_init.hpp"
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/program_options.hpp>
#include <unordered_map>


namespace bl = boost::log;
namespace fs = std::filesystem;
namespace po = boost::program_options;


template <typename T>
static auto make_sink(const boost::shared_ptr<T> & backend) {
    using sink_t = bl::sinks::synchronous_sink<T>;
    auto sink = boost::make_shared<sink_t>(backend);
    setup_sink_formatting(*sink);
    return sink;
}


void log_init(bool log_console, const fs::path & log_file) {

    if (!log_file.empty()) {
        fs::create_directories(log_file.parent_path());
        auto backend = boost::make_shared<bl::sinks::text_file_backend>(bl::keywords::file_name = log_file,
                                                                        bl::keywords::auto_flush = true);
        bl::core::get()->add_sink(make_sink(backend));
    }
    
    if (log_console) {
        auto backend = boost::make_shared<bl::sinks::text_ostream_backend>();
        backend->add_stream(boost::shared_ptr<std::ostream>(&std::cerr, boost::null_deleter()));
        bl::core::get()->add_sink(make_sink(backend));
    }

    bl::core::get()->set_filter(bl::trivial::severity >= bl::trivial::info);
}


/// Converts string to log severity level. Throws exception if string can't be converted
bl::trivial::severity_level str_to_sev_level(std::string_view str) {
    bl::trivial::severity_level slev = bl::trivial::severity_level::info;
    if (bl::trivial::from_string(str.data(), str.size(), slev)) {
        return slev;
    }

    std::ostringstream msg;
    msg << "invalid global log severity level: '" << str << "'";
    throw std::runtime_error(msg.str());
}


void log_init(const po::variables_map & vars,
              bool log_console,
              const fs::path & def_log_file) {
    fs::path log_file = vars.count("log-file") ? vars["log-file"].as<fs::path>() : def_log_file;
    log_init(log_console, log_file);

    if (vars.count("log-level") == 0) {
        return;
    }

    // parsing log level string

    auto log_level = vars["log-level"].as<std::string>();

    bl::trivial::severity_level global_sev = bl::trivial::severity_level::info;
    std::unordered_map<std::string, bl::trivial::severity_level> cat_sev_map;

    // splitting comma separated log level specifiers
    std::string_view log_level_sview = log_level;
    while (true) {
        auto comma_pos = log_level_sview.find(',');
        auto curr_spec = log_level_sview.substr(0, comma_pos);

        // trying find : in log level spec
        auto colon_pos = curr_spec.find(':');
        if (colon_pos == std::string_view::npos) {
            // global severity spec
            global_sev = str_to_sev_level(curr_spec);
        } else {
            auto cat_subcat_spec = curr_spec.substr(0, colon_pos);
            auto level_spec = curr_spec.substr(colon_pos + 1);
            cat_sev_map.emplace(cat_subcat_spec, str_to_sev_level(level_spec));
        }

        if (comma_pos == std::string_view::npos) {
            break;
        }

        log_level_sview.remove_prefix(comma_pos + 1);
    }


    // setting custom log filter

    auto filter = [global_sev, cat_sev_map](const bl::attribute_value_set & attrs) -> bool {
        // extracting severity
        bl::trivial::severity_level sev = bl::trivial::severity_level::info;
        auto sev_attr = attrs[bl::trivial::severity];
        if (!sev_attr.empty()) {
            sev = *sev_attr;
        }

        // extracting category and subcategory
        auto cat = attrs["Category"].extract_or_default<std::string, void, std::string>(std::string{});
        auto subcat = attrs["Subcategory"].extract_or_default<std::string, void, std::string>(std::string{});

        // first trying get level for subcategory
        if (!subcat.empty()) {
            auto it = cat_sev_map.find(cat + "/" + subcat);
            if (it != cat_sev_map.end()) {
                return sev >= it->second;
            }
        }

        // next truing get level for category
        auto it = cat_sev_map.find(cat);
        if (it != cat_sev_map.end()) {
            return sev >= it->second;
        }

        // checking global level
        return sev >= global_sev;
    };

    bl::core::get()->set_filter(filter);
}


po::options_description log_options() {
    po::options_description desc("Logging options");

    desc.add_options()
            ("log-level", po::value<std::string>(),
                "Logging level (trace, debug, info, warning, error, fatal)")
            ("log-file", po::value<fs::path>(), "Path to log file");

    return desc;
}
