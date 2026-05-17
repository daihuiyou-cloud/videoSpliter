cmake_minimum_required(VERSION 3.16)
project(videoSpliter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/dist)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/dist)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(videoSpliter WIN32
    src/main.cpp
    src/MainWindow.cpp
    src/SegmentModel.cpp
    src/FFmpegRunner.cpp
)

target_link_libraries(videoSpliter PRIVATE Qt5::Widgets)
target_compile_options(videoSpliter PRIVATE /utf-8)

add_custom_command(TARGET videoSpliter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/3rd/ffmpeg.exe
        $<TARGET_FILE_DIR:videoSpliter>/ffmpeg.exe
)
