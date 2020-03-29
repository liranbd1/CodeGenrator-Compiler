cmake_minimum_required(VERSION 3.15)
project(c_compiler_test)

set(CMAKE_C_STANDARD 90)

include_directories(compiler_files)

add_executable(c_compiler_test
        compiler_files/CodeGenerator.c
        compiler_files/ctree.c
        compiler_files/heap.c
        compiler_files/nmetab.c
        compiler_files/prnttree.c
        compiler_files/symtab.c
        compiler_files/token.c
        compiler_files/tree.c
        compiler_files/treestk.c
        compiler_files/dsm_extension.c
        compiler_files/lexer.c
        compiler_files/gram.c
        )
