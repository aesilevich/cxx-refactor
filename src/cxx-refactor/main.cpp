// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file main.cpp
/// Main entry point to cxx-refactor utility

#include "pch.hpp"
#include "find_definition_action.hpp"
#include "refactor_action.hpp"
#include "refactor_action_registry.hpp"
#include "template_parameter_remove_action.hpp"
#include "log/log_init.hpp"
#include <cm/src/cmsrc.hpp>
#include <cm/src/cxx/clang/cmsrcclang.hpp>
#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>


namespace fs = std::filesystem;
namespace po = boost::program_options;



int main(int argc, char * argv[]) {
    try {
        refactor_action_registry actions;
        actions.reg_action(std::make_unique<find_definition_action>());
        actions.reg_action(std::make_unique<template_parameter_remove_action>());

        po::options_description global_opts{"Global arguments"};
        global_opts.add_options()
            ("help", "Produce help message and exit")
            ("input,i", po::value<fs::path>()->required(), "path to input source to parse");
            // ("output,o", po::value<fs::path>(), "optional path to output source")
            // ("in-place", "overwrite original input files with changes")
        
        global_opts.add(log_options());

        po::options_description cmdline_opts{"Command line arguments"};
        cmdline_opts.add(global_opts);
        cmdline_opts.add_options()
            ("action", po::value<std::string>(), "Refactor action to perform")
            ("subargs", po::value<std::vector<std::string>>(), "Action specific arguments");

        po::positional_options_description pos_opts;
        pos_opts.add("action", 1);
        pos_opts.add("subargs", -1);

        po::variables_map var_map;
        po::command_line_parser parser(argc, argv);
        auto parsed_opts = parser
            .options(cmdline_opts)
            .positional(pos_opts)
            .allow_unregistered()
            .run();

        po::store(parsed_opts, var_map);

        // displaying help message if requested without action
        if (var_map.count("action") == 0) {
            std::cout << "cxx-refactor tool" << std::endl
                      << "Usage: cxx-refactor [global arguments] action [action arguments]"
                      << std::endl << std::endl;
            std::cout << global_opts << std::endl;

            std::cout << "Available refactor actions:" << std::endl;
            for (auto && act : actions.actions()) {
                std::cout << "  " << act->name() << std::endl;
            }
            return 1;
        }

        // searching for refactor action in actions registry
        auto & action = actions.find_action(var_map["action"].as<std::string>());

        // creating options descriptions for action
        auto act_opts = action.opts();

        // displaying help message for action if requested
        // (we need to do that before executing notify for gobal arguments)
        if (var_map.count("help") > 0) {
            std::cout << "cxx-refactor tool" << std::endl
                      << "Usage: cxx-refactor [global arguments] action [action arguments]"
                      << std::endl << std::endl
                      << global_opts << std::endl
                      << act_opts << std::endl;
            return 1;
        }

        notify(var_map);

        // collecting action options
        auto act_coll_opts = po::collect_unrecognized(parsed_opts.options, po::include_positional);

        // removing action name option
        act_coll_opts.erase(act_coll_opts.begin());

        // parsing action options
        po::variables_map act_var_map;
        po::store(po::command_line_parser(act_coll_opts).options(act_opts).run(), act_var_map);
        po::notify(act_var_map);

        // fs::path input;
        // if (opts.count("input") == 0) {
        //     throw std::runtime_error{"path to input source is not specified, "
        //                              "please set the --input option"};
        // }

        // input = opts["input"].as<fs::path>();

        // fs::path output;
        // if (opts.count("output") > 0) {
        //     output = opts["output"].as<fs::path>();
        // }

        // initializing log
        log_init(var_map);

        // constructing code model and parsing input source
        cm::src::source_code_model code_mdl;
        cm::src::clang::parse_source_file(code_mdl, var_map["input"].as<fs::path>(), {});

        // performing action
        action.perform(code_mdl, act_var_map);
    }
    catch (std::exception & err) {
        std::cerr << "ERROR: " << err.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "ERROR: unknown error" << std::endl;
        return 2;
    }

    return 0;
}
