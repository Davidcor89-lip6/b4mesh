cmake_minimum_required(VERSION 3.8)

message(STATUS "[b4mesh] Loading modules [${CMAKE_CURRENT_SOURCE_DIR}/modules] ...")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)

