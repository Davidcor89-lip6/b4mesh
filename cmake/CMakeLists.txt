cmake_minimum_required(VERSION 3.8)

# Fetch external CMake modules
# option(b4mesh_USE_EXTERNAL_LIBDBUSCXX "Use system libdbus ?" OFF)
# if (b4mesh_USE_EXTERNAL_LIBDBUSCXX)
#   ## FindDBusCXX
#   # or file(DOWNLOAD <url> [<file>] [<options>...])
#   include(FetchContent)
#   FetchContent_Declare(
#     FindDBusCXX
#     # URL https://raw.githubusercontent.com/adobe/webkit/master/Source/cmake/FindDBus.cmake
#     URL https://raw.githubusercontent.com/KDE/kimtoy/master/cmake/FindDBusCXX.cmake
#   )
#   FetchContent_MakeAvailable(FindDBusCXX)
#   FetchContent_GetProperties(FindDBusCXX)
# 
#   # include(${CMAKE_CURRENT_SOURCE_DIR}/external/FindDBusCXX.cmake)
# else()
#   # include(./dependencies_management/LibDBusCXX.cmake)
# endif()

include(./dependencies_management/greensoft_toolchain.cmake)
