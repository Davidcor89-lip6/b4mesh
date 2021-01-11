cmake_minimum_required(VERSION 3.8)
project(b4mesh VERSION 0.1 LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake) # Fetch external CMake modules

# todo : swap toolchain using option()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)         # ${PROJECT_NAME}::utils
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core_engine)   # ${PROJECT_NAME}::core_engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/binary)        # ${PROJECT_NAME}::binary

# todo : install() ...
# todo : test() + gtest dependencies
# todo : package() ...