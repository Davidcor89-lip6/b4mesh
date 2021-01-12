cmake_minimum_required(VERSION 3.8)
project(b4mesh VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # 20
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake) # Fetch external CMake modules

# todo : swap toolchain using option()
# option(b4mesh_)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)         # ${PROJECT_NAME}::utils
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core_engine)   # ${PROJECT_NAME}::core_engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/binary)        # ${PROJECT_NAME}::binary

# todo : install() ...
# todo : option(b4mesh_BUILD_TESTS) => test() + gtest dependencies
# todo : package() ...