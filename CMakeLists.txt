cmake_minimum_required(VERSION 3.23)
project(IFJ2 C)

set(CMAKE_C_STANDARD 99)

add_executable(IFJ2 main.c utilities/dyn_string.c utilities/dyn_string.h lexer.c lexer.h symtable.c symtable.h)
