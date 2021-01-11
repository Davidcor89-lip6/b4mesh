cmake_minimum_required(VERSION 3.8)

# Fetch external CMake modules
## FindDBusCXX
include(FetchContent)
FetchContent_Declare(
  FindDBusCXX
  # URL https://raw.githubusercontent.com/adobe/webkit/master/Source/cmake/FindDBus.cmake
  URL https://raw.githubusercontent.com/KDE/kimtoy/master/cmake/FindDBusCXX.cmake
)
FetchContent_GetProperties(FindDBusCXX)
if(NOT FindDBusCXX_POPULATED)
  FetchContent_Populate(FindDBusCXX)
endif()
FetchContent_MakeAvailable(FindDBusCXX)

# todo : create DBusCXX library IMPORTED target ?

include(${CMAKE_CURRENT_SOURCE_DIR}/FindDBusCXX.cmake)