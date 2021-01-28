cmake_minimum_required(VERSION 3.8)

message(STATUS "Generating toolchain from ${CMAKE_CURRENT_SOURCE_DIR} ...")
include(${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/libdbus-cpp.cmake)

