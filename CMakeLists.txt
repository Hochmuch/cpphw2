cmake_minimum_required(VERSION 3.29)
project(cpphw2)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpphw2 main.cpp
        fluid.h
        fixed.h
        vectorfield.h
        vectorfield.cpp
        fast_fixed.h
)
