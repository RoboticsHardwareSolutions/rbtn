cmake_minimum_required(VERSION 3.24)
project(rbtn
        VERSION 0.1.0
        LANGUAGES C
        DESCRIPTION
        "simple lib for work with button")


add_library(${PROJECT_NAME} STATIC rbtn.c rbtn.h rbtn_def.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (TARGET freertos_kernel)
    target_link_libraries(${PROJECT_NAME} PUBLIC freertos_kernel)
endif ()
