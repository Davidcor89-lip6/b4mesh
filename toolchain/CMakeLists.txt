cmake_minimum_required(VERSION 3.8)

include(${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.cmake)

if (NOT (TARGET greensoftsdk))
	message(FATAL_ERROR "[greensoftsdk] Target generation failed")
endif()
