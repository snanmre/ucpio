cmake_minimum_required(VERSION 3.0.0)
project(archive VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)

add_subdirectory(lib)
add_subdirectory(test)