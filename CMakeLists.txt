# Riccardo Zamuner

cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project(ldp_progetto_finale VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(include)

file(GLOB_RECURSE src_files "src/*.cpp")

add_executable(main ${src_files})

set(CMAKE_CXX_FLAGS "-O2")


