# Extract, build, and create targets related to GreenSoftSDK
#   thus generates ${PROJECT_SOURCE_DIR}/generated/GreenSoftSDK directory

# todo : force CMAKE_TOOLCHAIN_FILE to default when building GreenSoftSDK

# Extract target sources
file(GLOB GreenSoftSDK_filename
    LIST_DIRECTORIES false
    # CONFIGURE_DEPENDS
    greensoft-sdk-*.tar.xz
)
list(LENGTH GreenSoftSDK_filename GreenSoftSDK_filename_matches)
if (GreenSoftSDK_filename_matches NOT EQUAL 1)
    message(FATAL_ERROR "> Wrong GreenSoftSDK quantity : ${GreenSoftSDK_filename}")
endif()
list(GET GreenSoftSDK_filename 0 GreenSoftSDK_filename)

set(generated_dirname "${PROJECT_SOURCE_DIR}/generated")
if (NOT EXISTS "${generated_dirname}")
    file(MAKE_DIRECTORY ${generated_dirname})
endif()

set(GreenSoftSDK_extracted_dirname ${generated_dirname}/GreenSoftSDK)
add_custom_target(greenSoftSDK_extract
    COMMAND cmake -E tar xzf ${GreenSoftSDK_filename} -- ${GreenSoftSDK_extracted_dirname}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # useless ?
    OUTPUTS GreenSoftSDK
)

# Create and/or build target
add_custom_target(greenSoftSDK_build
    COMMAND "make alldefconfig" # OK config ?
    COMMAND "make"
    DEPENDS greenSoftSDK_extract
    BYPRODUCTS
        ./output/host/g++
        ./output/host/arm-linux-g++
        # ./usr/lib/dbus-c++-1/
        # ./usr/include/dbus-c++-1/
    WORKING_DIRECTORY ${GreenSoftSDK_extracted_dirname}
)

# GreenSoftSDK::DBusCXX
add_library(DBusCXX SHARED IMPORTED GLOBAL)
add_dependencies(DBusCXX greenSoftSDK_build)
set_target_properties(DBusCXX PROPERTIES
  IMPORTED_LOCATION         "${GreenSoftSDK_extracted_dirname}/usr/lib/dbus-c++-1.so"
  IMPORTED_LOCATION_DEBUG   "${GreenSoftSDK_extracted_dirname}/usr/lib/dbus-c++-1-d.so" # ?
  IMPORTED_CONFIGURATIONS   "RELEASE;DEBUG"
)
target_include_directories(DBusCXX PUBLIC
    ${GreenSoftSDK_extracted_dirname}/usr/include/dbus-c++-1
)
add_library(DBusCXX ALIAS GreenSoftSDK::DBusCXX)

# Toolchains
# add_custom_target(greenSoftSDK_toolchain_host)
# add_custom_target(greenSoftSDK_toolchain_local)

# Generates toolchain files for -DCMAKE_TOOLCHAIN_FILE
# Force toolchain
#   SET(CMAKE_TOOLCHAIN_FILE  "${GreenSoftSDK_toolchain_path}" CACHE INTERNAL "CMAKE_TOOLCHAIN_FILE")