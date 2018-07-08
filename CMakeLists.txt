cmake_minimum_required(VERSION 3.6.0)

project(screenrecord)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/out)

include_directories(${PROJECT_SOURCE_DIR}/depends/libyuv/include)
include_directories(${PROJECT_SOURCE_DIR}/depends/x264/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/ SOURCE_DIR)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(screenrecord ${SOURCE_DIR})

target_link_libraries(screenrecord
    xcb
    xcb-shm
    pthread
    dl
    ${PROJECT_SOURCE_DIR}/depends/x264/lib/libx264.a
    ${PROJECT_SOURCE_DIR}/depends/libyuv/lib/libyuv.a
    )