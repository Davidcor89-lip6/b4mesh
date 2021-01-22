message(STATUS "[greensoftsdk] Generating ...")

# Extract, build, and create targets related to GreenSoftSDK
#   thus generates ${PROJECT_SOURCE_DIR}/generated/GreenSoftSDK directory

# todo : force CMAKE_TOOLCHAIN_FILE to default when building GreenSoftSDK

# Extract target sources

# set(greenSoftSDK_archive_directory ${PROJECT_SOURCE_DIR}/archives)
# message(STATUS "Scanning [${greenSoftSDK_archive_directory}] for [greensoft-sdk-*.tar.xz] ...")
# file(GLOB GreenSoftSDK_filename
#     LIST_DIRECTORIES false
#     # CONFIGURE_DEPENDS
#     ${greenSoftSDK_archive_directory}/greensoft-sdk-*.tar.xz
# )
# 
# list(LENGTH GreenSoftSDK_filename GreenSoftSDK_filename_matches)
# if (NOT(${GreenSoftSDK_filename_matches} EQUAL 1))
#     message(FATAL_ERROR "> Wrong GreenSoftSDK quantity : ${GreenSoftSDK_filename}")
# endif()
# list(GET GreenSoftSDK_filename 0 GreenSoftSDK_filename)
# message(STATUS "\tFound [${GreenSoftSDK_filename}]")
# # Use ${GreenSoftSDK_filename} as ExternalProject_Add URL value ...

# include(ExternalProject)
# find_program(MAKE_EXE NAMES gmake nmake make)
# ExternalProject_Add(greenSoftSDK
#   URL               https://github.com/Davidcor89-lip6/b4mesh/raw/main/archives/greensoft-sdk-2020-10-06-67b51.tar.xz
#   CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "[greenSoftSDK] Starting $<CONFIG> configuration ..."
#   COMMAND           ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/archives/libdbus-cpp-6d390205.tar.gz <BUILD_DIR>/dl/
#   COMMAND           ${MAKE_EXE} alldefconfig
#   BUILD_COMMAND     ${CMAKE_COMMAND} -E echo "[greenSoftSDK] Starting $<CONFIG> build ..."
#   COMMAND           ${MAKE_EXE}
#   BUILD_BYPRODUCTS # useless ?
#     <INSTALL_DIR>/usr/lib/dbus-c++-1.so
#     <INSTALL_DIR>/usr/lib/dbus-c++-1-d.so
#     <INSTALL_DIR>/usr/include/dbus-c++-1
# )
# ExternalProject_Get_Property(greenSoftSDK install_dir)
# set(greenSoftSDK_install_dir ${install_dir})

include(FetchContent)
FetchContent_Declare(greensoftsdk # Consistency : as FetchContent generated targets are lowercase
    PREFIX  external_dependencies/greensoftsdk
    URL     ${PROJECT_SOURCE_DIR}/archives/greensoft-sdk-2020-10-06-67b51.tar.xz #https://github.com/Davidcor89-lip6/b4mesh/raw/main/archives/greensoft-sdk-2020-10-06-67b51.tar.xz
) 
FetchContent_GetProperties(greensoftsdk)
if(NOT greensoftsdk_POPULATED)
    FetchContent_Populate(greensoftsdk)

    find_program(MAKE_EXE NAMES gmake nmake make)
    message(STATUS "[greensoftsdk] Found MAKE_EXE=[${MAKE_EXE}]")

    # todo : split targets
    # make libdbus-cpp
    # make toolchain
    # todo : execute_process => custom_target

    execute_process(
        COMMAND_ECHO STDOUT
        COMMAND ${CMAKE_COMMAND} -E echo "[greensoftsdk] Starting prerequisites (custom libdbus-c++ integration) ..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/archives/libdbus-cpp-6d390205.tar.gz ${greensoftsdk_SOURCE_DIR}/dl
        COMMAND ${CMAKE_COMMAND} -E echo "[greensoftsdk] Starting configuration ..."
        COMMAND ${MAKE_EXE} alldefconfig
        COMMAND ${CMAKE_COMMAND} -E echo "[greensoftsdk] Starting build ..."
        COMMAND ${MAKE_EXE}

        WORKING_DIRECTORY ${greensoftsdk_SOURCE_DIR}
        OUTPUT_FILE       ${greensoftsdk_BINARY_DIR}/build_output.log
        ERROR_FILE        ${greensoftsdk_BINARY_DIR}/build_output.log
        RESULT_VARIABLE   result
    )
    message(STATUS "[greensoftsdk] Build complete")
    if (result)
        message(FATAL_ERROR "[greensoftsdk] Build failed, see log at:\t${greensoftsdk_BINARY_DIR}/build_output.log")
    endif()
endif()

# set(generated_dirname "${PROJECT_SOURCE_DIR}/generated")
# if (NOT EXISTS "${generated_dirname}")
#     file(MAKE_DIRECTORY ${generated_dirname})
# endif()

# set(GreenSoftSDK_extracted_dirname ${generated_dirname}/GreenSoftSDK)
# # Extract tarball target
# add_custom_command(OUTPUT ${GreenSoftSDK_extracted_dirname}
#     COMMAND echo -e "Extracting greenSoftSDK from [${GreenSoftSDK_filename}] ..."
#     COMMAND cmake -E tar xzf ${GreenSoftSDK_filename} -- ${GreenSoftSDK_extracted_dirname}
#     COMMAND echo -e "Extracting greenSoftSDK... done"
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # useless ?
#     DEPENDS ${GreenSoftSDK_filename}
#     BYPRODUCTS
#         ${GreenSoftSDK_extracted_dirname}
#         ${GreenSoftSDK_extracted_dirname}/Makefile
# )
# add_custom_target(greenSoftSDK_extract
#     DEPENDS
#         ${GreenSoftSDK_extracted_dirname}
#         ${GreenSoftSDK_extracted_dirname}/Makefile
# )

# # Create and/or build target
# add_custom_command(OUTPUT ${GreenSoftSDK_extracted_dirname}/usr
#     DEPENDS greenSoftSDK_extract
#     COMMAND echo -e "Building greenSoftSDK..."
#     COMMAND make alldefconfig # OK config ?
#     COMMAND make
#     COMMAND echo -e "Building greenSoftSDK... done"
#     BYPRODUCTS
#         ${GreenSoftSDK_extracted_dirname}/output/host/g++           # a confirmer
#         ${GreenSoftSDK_extracted_dirname}/output/host/arm-linux-g++ # a confirmer
#         ${GreenSoftSDK_extracted_dirname}/usr                       # a confirmer
#     WORKING_DIRECTORY ${GreenSoftSDK_extracted_dirname}
# )
# add_custom_target(greenSoftSDK_build
#     DEPENDS
#         ${GreenSoftSDK_extracted_dirname}/usr
# )

# GreenSoftSDK::DBusCXX

add_library(DBusCXX SHARED IMPORTED GLOBAL)
set_target_properties(DBusCXX PROPERTIES
    IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
    IMPORTED_LOCATION_RELEASE       ${greenSoftSDK_install_dir}/usr/lib/dbus-c++-1.so
    IMPORTED_LOCATION_DEBUG         ${greenSoftSDK_install_dir}/usr/lib/dbus-c++-1-d.so
    # INTERFACE_INCLUDE_DIRECTORIES   ${greenSoftSDK_install_dir}/usr/include/dbus-c++-1
)
add_dependencies(DBusCXX greenSoftSDK)
add_library(GreenSoftSDK::DBusCXX ALIAS DBusCXX)

# Toolchains
# add_custom_target(greenSoftSDK_toolchain_host)
# add_custom_target(greenSoftSDK_toolchain_local)

# Generates toolchain files for -DCMAKE_TOOLCHAIN_FILE
# Force toolchain
#   SET(CMAKE_TOOLCHAIN_FILE  "${GreenSoftSDK_toolchain_path}" CACHE INTERNAL "CMAKE_TOOLCHAIN_FILE")

message(STATUS "[GreenSoftSDK] Generating ... done")