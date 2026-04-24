cmake_minimum_required(VERSION 3.26)
project(P0_c C)

set(CMAKE_C_STANDARD 11)

add_executable(P0_c main.c
        linkedlist.h
        linkedlist.c
        cmd.h
        cmd.c
        auxiliar.c
        auxiliar.h
        auxiliar.c)
