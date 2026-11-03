cmake_minimum_required(VERSION 3.20)
project(sovereign_kernel CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mavx2 -march=native")

include_directories(include)
add_executable(test_kernel tests/test_sovereign_kernel.cpp)
