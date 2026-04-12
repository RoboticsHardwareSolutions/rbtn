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

if(TARGET freertos_kernel)
        target_link_libraries(${PROJECT_NAME} PUBLIC freertos_kernel)
endif()

#################### FORMAT SECTION #######################

execute_process(
        COMMAND curl --max-time 2 -Is https://github.com
        RESULT_VARIABLE GITHUB_CONNECT_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
)
if(GITHUB_CONNECT_RESULT EQUAL 0)
        include(FetchContent)
                FetchContent_Declare(
                        rlibhelper
                        GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rlibhelper.git
                        GIT_TAG main
                        SOURCE_DIR "${CMAKE_SOURCE_DIR}/_deps/rlibhelper-src"
                        BINARY_DIR "${CMAKE_SOURCE_DIR}/_deps/rlibhelper-build"
                        SUBBUILD_DIR  "${CMAKE_SOURCE_DIR}/_deps/rlibhelper-subbuild"
                )
        FetchContent_MakeAvailable(rlibhelper)
else()
        message(WARNING "No internet connection: rlibhelper will not be downloaded. Formatting may not work.")
endif()
set(FILES_FOR_FORMATTING
        rbtn.c
        rbtn.h
        rbtn_def.h
)

if(EXISTS "${rlibhelper_SOURCE_DIR}/format.cmake")
        include(${rlibhelper_SOURCE_DIR}/format.cmake)
        format_files(${PROJECT_NAME} SOURCES ${FILES_FOR_FORMATTING})
elseif(EXISTS "${CMAKE_SOURCE_DIR}/_deps/rlibhelper-src/format.cmake")
        include(${CMAKE_SOURCE_DIR}/_deps/rlibhelper-src/format.cmake)
        format_files(${PROJECT_NAME} SOURCES ${FILES_FOR_FORMATTING})
else()
        message(WARNING "format.cmake file not found")
endif()
