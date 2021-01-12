
set(greensoft_sdk_dirname ${CMAKE_CURRENT_PROJECT_DIR}/greensoft_sdk)

option(b4mesh_ALWAYS_generates_GreenSoftSDK "Always generates GreenSoftSDK ? ON=YES, OFF=ONLY_ONCE" OFF)

# todo : force CMAKE_TOOLCHAIN_FILE to default for GreenSoftSDK build

# Extract target sources
if (${b4msh_ALWAYS_generates_GreenSoftSDK} OR
    NOT EXISTS "${greensoft_sdk_dirname}") # todo : better condition here, "refresh if newer, etc"

    file(MAKE_DIRECTORY ${greensoft_sdk_dirname})
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

    file(ARCHIVE_EXTRACT INPUT "${GreenSoftSDK_filename}"
        DESTINATION ${greensoft_sdk_dirname}
        # [PATTERNS <patterns>...]
        # [LIST_ONLY]
        # [VERBOSE]
    )
    get_filename_component(GreenSoftSDK_extracted_dirname ${GreenSoftSDK_filename} NAME_WE)

    add_custom_target(greenSoftSDK_extract
        COMMAND cmake -E tar xzf ...
        WORKING_DIRECTORY ${greensoft_sdk_dirname}
        OUTPUTS
    )
endif()
# Create and/or build target
add_custom_target(greenSoftSDK_build
    COMMAND "make alldefconfig"
    COMMAND "make"
    # [DEPENDS depend depend depend ... ]
    BYPRODUCTS
        ./output/host/g++
        ./output/host/arm-linux-g++
        # ./usr/lib/dbus-c++-1/
        # ./usr/include/dbus-c++-1/
    WORKING_DIRECTORY ${greensoft_sdk_dirname}
)

# GreenSoftSDK libDBusC++
add_library(DBus SHARED IMPORTED GLOBAL)
add_dependencies(DBus greenSoftSDK_build)
set_target_properties(DBus PROPERTIES
  IMPORTED_LOCATION         "${greensoft_sdk_dirname}/usr/lib/dbus-c++-1.so"
  IMPORTED_LOCATION_DEBUG   "${greensoft_sdk_dirname}/usr/lib/dbus-c++-1-d.so" # ?
  IMPORTED_CONFIGURATIONS   "RELEASE;DEBUG"
)
target_include_directories(DBus PUBLIC
    ${greensoft_sdk_dirname}/usr/include/dbus-c++-1
)
add_library(DBus ALIAS GreenSoftSDK::LibDBus)

# Toolchains
# add_custom_target(greenSoftSDK_toolchain_host)
# add_custom_target(greenSoftSDK_toolchain_local)

# Generates toolchain files for -DCMAKE_TOOLCHAIN_FILE