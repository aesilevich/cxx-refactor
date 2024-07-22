
# cxx-refactor project

cmake_minimum_required(VERSION 3.24)
project(cxx-refactor)

option(CXX_REFACTOR_BUILD_BOOST "Build Boost library from sources" ON)
option(CXX_REFACTOR_BUILD_LLVM "Build LLVM from sources" ON)


include(CTest)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

# Boost library build
if("${CXX_REFACTOR_BUILD_BOOST}")
    FetchContent_Declare(
        Boost
        URL "https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.tar.gz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(Boost)
endif()


# LLVM Library build
if("${CXX_REFACTOR_BUILD_LLVM}")
    FetchContent_Declare(
        LLVM
        URL "https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/llvm-project-18.1.8.src.tar.xz"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(LLVM)

    FetchContent_GetProperties(LLVM SOURCE_DIR llvm_src_dir BINARY_DIR llvm_binary_dir)

    # fake config for Clang package
    file(MAKE_DIRECTORY "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}")
    file(TOUCH "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/clang-config.cmake")

    set(LLVM_ENABLE_PROJECTS "clang" CACHE STRING "LLVM projects to build")
    set(LLVM_ENABLE_RUNTIMES "libcxx;libcxxabi;libunwind" CACHE STRING "LLVM runtimes to build")

    message(STATUS "LLVM SOURCE DIR: ${llvm_src_dir}")
    message(STATUS "LLVM BINARY DIR: ${llvm_binary_dir}")
    add_subdirectory("${llvm_src_dir}/llvm" "${llvm_binary_dir}")

    target_include_directories(libclang
        INTERFACE "$<BUILD_INTERFACE:${llvm_src_dir}/clang/include>"
                  "$<BUILD_INTERFACE:${llvm_src_dir}/llvm/include>"
                  "$<BUILD_INTERFACE:${llvm_binary_dir}/tools/clang/include>"
                  "$<BUILD_INTERFACE:${llvm_binary_dir}/include>"
    )
endif()


# Boost.Test is required for building tests
find_package(Boost 1.64 REQUIRED COMPONENTS
             unit_test_framework
             program_options
             log)

add_subdirectory(cm)
add_subdirectory(src)
