cmake_minimum_required (VERSION 2.8)
project (rang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${PROJECT_SOURCE_DIR}/coveralls-cmake/cmake)

if (COVERALLS)
	include(Coveralls)
	coveralls_turn_on_coverage()
endif()


if (COVERALLS)
	set(COVERAGE_SRCS ${PROJECT_SOURCE_DIR}/include/rang.hpp ${PROJECT_SOURCE_DIR}/test/test.cpp)
	coveralls_setup(
        "${COVERAGE_SRCS}"
        ON)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
enable_testing()

include_directories(include)
set(CATCH_HEADER_PATH ${PROJECT_SOURCE_DIR}/test/catch.hpp)

if(BUILD_TESTING)
    add_executable(vTest ${PROJECT_SOURCE_DIR}/test/visualTest.cpp)
    add_executable(runTest ${PROJECT_SOURCE_DIR}/test/test.cpp ${CATCH_HEADER_PATH})
    add_executable(allColors ${PROJECT_SOURCE_DIR}/test/colortest.cpp)
    add_executable(allInput ${PROJECT_SOURCE_DIR}/test/inputtest.cpp
        ${CATCH_HEADER_PATH})

    add_test(RTEST ${CMAKE_BINARY_DIR}/bin/runTest)
endif()

install(FILES ${PROJECT_SOURCE_DIR}/include/rang.hpp DESTINATION include)
