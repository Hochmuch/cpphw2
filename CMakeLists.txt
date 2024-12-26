cmake_minimum_required(VERSION 3.29)
project(cpphw2)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpphw2 main.cpp
        fluid.h
        fixed.h
        vectorfield.h
        vectorfield.cpp
        fast_fixed.h
        reader.h
        basic_fixed.h
        precalc.h
)
add_compile_definitions("TYPES=DOUBLE,FIXED(16,4),FLOAT")
add_compile_definitions("SIZES=Size_pair<36, 84>, Size_pair<1, 1>")
