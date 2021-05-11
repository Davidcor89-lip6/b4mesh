if (NOT (DEFINED PROJECT_NAME) OR ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}"))
    message(FATAL_ERROR "b4mesh/sources used as a top-level CMake project. Use b4mesh/ instead.")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)         # ${PROJECT_NAME}::utils
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core_engine)   # ${PROJECT_NAME}::core_engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/binary)        # ${PROJECT_NAME}::binary
