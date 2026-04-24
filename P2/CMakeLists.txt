cmake_minimum_required(VERSION 3.29)
project(P3 C)

set(CMAKE_C_STANDARD 11)

add_executable(P1 p3.c
        prompt.c
        prompt.h
        cmd.c
        cmd.h
        auxfunctions.c
        auxfunctions.h
        linkedlist.c
        linkedlist.h
        processCommand.c
        processCommand.h
        memoryBlocks.h)
