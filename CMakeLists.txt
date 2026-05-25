cmake_minimum_required(VERSION 3.29)
project(map_reduce_C C)

set(CMAKE_C_STANDARD 11)

add_executable(map_reduce_C src/main.c
        src/mapreduce.h
        src/mapreduce.c)
