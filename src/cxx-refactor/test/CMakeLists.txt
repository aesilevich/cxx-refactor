

# Code model clang builder test
add_executable(cxx-refactor-test
               test.cpp
               source_rewriter_test.cpp
              )

target_link_libraries(cxx-refactor-test PRIVATE cxx-refactor-lib
                                                Boost::unit_test_framework)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(cxx-refactor-test PRIVATE rt)
endif()

add_test(NAME cxx-refactor-test COMMAND cxx-refactor-test)
