
# Logging utilities library for refactoring tool
add_library(refactor-log
            log_init.cpp)
target_link_libraries(refactor-log PRIVATE
                      Boost::program_options
                      Boost::log)
target_include_directories(refactor-log PUBLIC "..")
