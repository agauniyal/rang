cmake_minimum_required(VERSION 2.8.12)

project(rang-test)

set(CMAKE_CXX_STANDARD          11 )
set(CMAKE_CXX_STANDARD_REQUIRED ON )
set(CMAKE_CXX_EXTENSIONS        OFF)

function(rang_add_test file_name)
    add_executable("${file_name}" "${file_name}.cpp")
    target_link_libraries("${file_name}" rang)
endfunction()

# simple tests #################################################################

rang_add_test(colorTest)
rang_add_test(envTermMissing)

# test that uses doctest #######################################################

set(doctest_DIR "" CACHE PATH "Directory containing doctestConfig.cmake")
find_package(doctest)

if (${doctest_FOUND} EQUAL 1)
    add_executable(all_rang_tests "test.cpp")
    target_link_libraries(all_rang_tests rang doctest::doctest)

    enable_testing()

    # cd build_dir && ctest --test-command all_tests
    add_test(NAME all_tests COMMAND "$<TARGET_FILE:all_rang_tests>")
endif()
