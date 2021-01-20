# Extract, build, and create targets related to LibDBusCXX
#   thus generates ${PROJECT_SOURCE_DIR}/generated/LibDBusCXX directory

# Extract target sources
set(libDBusCXX_archive_directory ${PROJECT_SOURCE_DIR}/archives)
message(STATUS "Scanning [${libDBusCXX_archive_directory}] for [greensoft-sdk-*.tar.xz] ...")
file(GLOB libDBusCXX_filename
    LIST_DIRECTORIES false
    # CONFIGURE_DEPENDS
    ${libDBusCXX_archive_directory}/libdbus-cpp-*.tar.gz
)

list(LENGTH libDBusCXX_filename libDBusCXX_filename_matches)
if (NOT(${libDBusCXX_filename_matches} EQUAL 1))
    message(FATAL_ERROR "> Wrong libDBusCXX quantity : ${libDBusCXX_filename}")
endif()
list(GET libDBusCXX_filename 0 libDBusCXX_filename)
message(STATUS "\tFound [${libDBusCXX_filename}]")


# or just add_subdirectory + command make_exe

include(ExternalProject)
find_program(MAKE_EXE NAMES gmake nmake make)
ExternalProject_Add(libDBusCXX_build
  URL               ${libDBusCXX_filename}
  # PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/project_a
  # CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ${MAKE_EXE} alldefconfig && make
)


# add_library(DBusCXX SHARED IMPORTED GLOBAL) # SHARED
# add_dependencies(DBusCXX libDBusCXX_build)
# target_link_libraries(DBusCXX INTERFACE libDBusCXX_build)
# set_target_properties(DBusCXX PROPERTIES
#   IMPORTED_LOCATION         "${libDBusCXX_extracted_dirname}/usr/lib/dbus-c++-1.so"
#   IMPORTED_LOCATION_DEBUG   "${libDBusCXX_extracted_dirname}/usr/lib/dbus-c++-1-d.so" # ?
#   IMPORTED_CONFIGURATIONS   "RELEASE;DEBUG"
# )
# target_include_directories(DBusCXX INTERFACE
#     ${libDBusCXX_extracted_dirname}/usr/include/dbus-c++-1
# )
# add_library(libDBusCXX::DBusCXX ALIAS DBusCXX)
