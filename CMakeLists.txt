cmake_minimum_required(VERSION 3.8)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake) # modules, dependencies
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/toolchain)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Default build type")
endif()

project(b4mesh
    VERSION 0.1
    LANGUAGES CXX
    DESCRIPTION "B4Mesh main projet"
    #HOMEPAGE_URL https://github.com/Davidcor89-lip6/b4mesh # WTF mix-up with CMAKE_C_COMPILER on linux ???
)
enable_testing()

message(STATUS "[${PROJECT_NAME}] : CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17) # 20
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(Boost_USE_DEBUG_LIBS OFF)
# set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

# todo : swap toolchain using option()
# option(b4mesh_)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)         # ${PROJECT_NAME}::utils
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core_engine)   # ${PROJECT_NAME}::core_engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/binary)        # ${PROJECT_NAME}::binary

# todo : install() ... include(GNUInstallDirs)
# todo : option(b4mesh_BUILD_TESTS) => test() + gtest dependencies
# todo : package() ...