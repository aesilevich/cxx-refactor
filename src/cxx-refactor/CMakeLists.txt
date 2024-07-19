
find_package(Boost 1.64 REQUIRED COMPONENTS program_options)


add_library(cxx-refactor-lib
            find_definition_action.cpp
            source_rewriter.cpp
            source_modification_action.cpp
            template_parameter_remove_action.cpp)
target_link_libraries(cxx-refactor-lib PUBLIC cm-src-cxx-clang)
target_precompile_headers(cxx-refactor-lib PRIVATE pch.hpp)
target_link_libraries(cxx-refactor-lib PRIVATE Boost::program_options)

add_executable(cxx-refactor
               main.cpp)
target_precompile_headers(cxx-refactor PRIVATE pch.hpp)
target_link_libraries(cxx-refactor PRIVATE cxx-refactor-lib Boost::program_options)


add_subdirectory(test)
