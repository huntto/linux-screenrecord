cmake_minimum_required(VERSION 3.6.0)

project(screenrecord)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/out)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ SOURCE_DIR)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(screenrecord ${SOURCE_DIR})