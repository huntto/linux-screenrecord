cmake_minimum_required(VERSION 3.6.0)

project(screenrecord)

add_definitions(-Wunused-parameter)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/out)

include_directories(${PROJECT_SOURCE_DIR}/depends/include)

if(APPLE)
    include_directories(/usr/X11/include)
    link_directories(/usr/X11/lib/)
    set(LINK_DIR ${PROJECT_SOURCE_DIR}/depends/lib/mac)
endif()

if(UNIX AND NOT APPLE)
    set(LINK_DIR ${PROJECT_SOURCE_DIR}/depends/lib/linux)
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/video/ SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/audio/ SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/avformat/ SOURCE_DIR)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(screenrecord ${SOURCE_DIR})

target_link_libraries(screenrecord
    xcb
    xcb-shm
    pthread
    dl
    pulse
    pulse-simple
    ${LINK_DIR}/libx264.a
    ${LINK_DIR}/libyuv.a
    ${LINK_DIR}/libfaac.a
    )
