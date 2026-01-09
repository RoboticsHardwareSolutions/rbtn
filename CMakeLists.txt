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

#################### FORMAT SECTION #######################
include(FetchContent)
FetchContent_Declare(
        rlibhelper
        GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rlibhelper.git
        GIT_TAG main
)
FetchContent_MakeAvailable(rlibhelper)
set(FILES_FOR_FORMATTING
        rbtn.c
        rbtn.h
        rbtn_def.h
)

if (EXISTS "${rlibhelper_SOURCE_DIR}/format.cmake")
    include(${rlibhelper_SOURCE_DIR}/format.cmake)
else ()
    message(WARNING "format.cmake file not found")
endif ()
format_files(${PROJECT_NAME} SOURCES ${FILES_FOR_FORMATTING})