cmake_minimum_required(VERSION 3.24)
project(rbtn C)
set(CMAKE_C_STANDARD 11)

add_library(${PROJECT_NAME} rbtn.c rbtn.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})


