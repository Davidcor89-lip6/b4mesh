cmake_minimum_required(VERSION 3.8)

message(STATUS "Generating toolchain from ${CMAKE_CURRENT_SOURCE_DIR} ...")
include(${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.cmake)
#include(dependencies)

if (NOT (TARGET greensoftsdk))
	message(FATAL_ERROR "[greensoftsdk] Target export failed")
endif()
#if (NOT (TARGET GreenSoftSDK::DBusCXX))
#	message(FATAL_ERROR "[GreenSoftSDK::DBusCXX] Target export failed")
#endif()

message("Setting toolchain to : [${greensoftsdk_toolchain_descriptor}]")
if (NOT EXISTS "${greensoftsdk_toolchain_descriptor}")
	message(FATAL_ERROR "Cannot find toolchain descriptor : [${greensoftsdk_toolchain_descriptor}]")
endif()
SET(CMAKE_TOOLCHAIN_FILE "${greensoftsdk_toolchain_descriptor}"
	CACHE INTERNAL "greensoftsdk cmake toolchain descriptor"
)