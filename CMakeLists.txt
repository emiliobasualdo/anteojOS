cmake_minimum_required(VERSION 3.10)
project(anteojOS)

set(CMAKE_CXX_STANDARD 11)

include_directories(Kernel/include)
include_directories(Toolchain/ModulePacker)
include_directories(Userland/SampleCodeModule/include)

add_executable(anteojos
        Bootloader
        Kernel
        Toolchain
        Userland
        Dockerfile
        Makefile
        Kernel/beepDriver.c)
