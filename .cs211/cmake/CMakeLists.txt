cmake_minimum_required(VERSION 3.3)

if(NOT DOT_CS211)
    set(DOT_CS211 "${PROJECT_SOURCE_DIR}/.cs211")
endif()

list(APPEND CMAKE_MODULE_PATH "${DOT_CS211}/cmake")
include(211helpers)
include(211commands)

enable_testing()

find_local_package(Catch2 ${DOT_CS211}/lib/catch VERSION 2020.2)
find_local_package(Ge211  ${DOT_CS211}/lib/ge211 VERSION 2021.3)

include_directories(src)

# vim: ft=cmake
