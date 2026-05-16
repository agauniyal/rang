cmake_minimum_required(VERSION 3.10)
project(rang_test CXX)

find_package(rang REQUIRED)

add_executable(visualTest visualTest.cpp)
target_link_libraries(visualTest PRIVATE rang::rang)
