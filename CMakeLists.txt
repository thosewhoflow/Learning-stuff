cmake_minimum_required(VERSION 4.3)
project(malloc C)

set(CMAKE_C_STANDARD 11)

add_executable(malloc main.c
        my_malloc/my_malloc.c
        my_malloc/my_malloc.h
        my_free/my_free.c
        my_free/my_free.h
        debug/debug_stuff.c)
