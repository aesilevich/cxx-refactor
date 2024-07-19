
# cxx-refactor project

cmake_minimum_required(VERSION 3.6)
project(cxx-refactor)

include(CTest)

set(CMAKE_CXX_STANDARD 20)

# Boost.Test is required for building tests
if("${BUILD_TESTING}")
    find_package(Boost 1.64 REQUIRED COMPONENTS "unit_test_framework")
endif()

add_subdirectory(cm)
add_subdirectory(src)
