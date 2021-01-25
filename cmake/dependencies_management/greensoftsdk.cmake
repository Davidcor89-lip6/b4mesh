message(STATUS "[greensoftsdk] Generating ...")

# Extract, build, and create targets related to GreenSoftSDK
#   thus generates ${PROJECT_SOURCE_DIR}/generated/GreenSoftSDK directory

# Extract target sources
function(greensoftsdk_FindArchiveFile)
# Set `greensoftsdk_FindArchiveFile_result`

    set(options)
    set(oneValueArgs DIRNAME PATTERN)
    set(multiValueArgs)
    cmake_parse_arguments(GREENSOFTSDK_FINDARCHIVEFILE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # set(greensoftsdk_archiveDirectory ${PROJECT_SOURCE_DIR}/archives)
    # set(greensoftsdk_archiveFilenamePattern "greensoft-sdk-*.tar.xz")
    message(STATUS "[greensoftsdk_FindArchiveFile] Scanning [${GREENSOFTSDK_FINDARCHIVEFILE_DIRNAME}] for [${GREENSOFTSDK_FINDARCHIVEFILE_PATTERN}] ...")
    file(GLOB filesnames
        LIST_DIRECTORIES false
        # CONFIGURE_DEPENDS
        ${GREENSOFTSDK_FINDARCHIVEFILE_DIRNAME}/${GREENSOFTSDK_FINDARCHIVEFILE_PATTERN}
    )

    list(LENGTH filesnames GreenSoftSDK_filename_matches)
    if (NOT(${GreenSoftSDK_filename_matches} EQUAL 1))
        message(FATAL_ERROR "[greensoftsdk_FindArchiveFile] Wrong GreenSoftSDK quantity : ${filesnames}")
    endif()
    list(GET filesnames 0 filesnames)

    message(STATUS "[greensoftsdk_FindArchiveFile] Found [${filesnames}]")
    set(greensoftsdk_FindArchiveFile_result ${filesnames} PARENT_SCOPE)
endfunction()

macro(greensoftsdk_builder)
    set(options)
    set(oneValueArgs    TAG_INFO)
    set(multiValueArgs  EXECUTEPROCESS_ARGS)
    cmake_parse_arguments(GREENSOFTSDK_BUILDER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    message(STATUS "[greensoftsdk](${GREENSOFTSDK_BUILDER_TAG_INFO}) ...")

    set(output_filepath ${greensoftsdk_BINARY_DIR}/build_output.${GREENSOFTSDK_BUILDER_TAG_INFO}.log)

    execute_process(
        COMMAND_ECHO STDOUT
        ${GREENSOFTSDK_BUILDER_EXECUTEPROCESS_ARGS}

        WORKING_DIRECTORY ${greensoftsdk_SOURCE_DIR}
        OUTPUT_FILE       ${output_filepath}
        ERROR_FILE        ${output_filepath}
        RESULT_VARIABLE   result
    )
    message(STATUS "[greensoftsdk](${GREENSOFTSDK_BUILDER_TAG_INFO}) complete")
    if (result)
        message(FATAL_ERROR "[greensoftsdk](${GREENSOFTSDK_BUILDER_TAG_INFO}) Build failed\nsee log at:\t${output_filepath}")
    endif()
endmacro()

macro(greensoftsdk_generateToolchain)

    # greensoftsdk_builder(TAG_INFO "configuration"
    #     EXECUTEPROCESS_ARGS
    #         COMMAND ${MAKE_EXE} alldefconfig
    # )
    greensoftsdk_builder(TAG_INFO "toolchain"
        EXECUTEPROCESS_ARGS
            COMMAND ${MAKE_EXE} toolchain
    )
endmacro()

macro(greensoftsdk_generateLibDBusCXX)

    greensoftsdk_FindArchiveFile(
        DIRNAME ${PROJECT_SOURCE_DIR}/archives
        PATTERN "libdbus-cpp-*.tar.gz"
    )
    set(greensoftsdk_libdbuscpp_tarball_path ${greensoftsdk_FindArchiveFile_result})

    file(SHA256
        ${greensoftsdk_libdbuscpp_tarball_path}
        "6842e99baf73372ae8d047c3b2d79ca2f5d57f900cb436890a9a8ac19930b411"
    ) # as arg

    greensoftsdk_builder(TAG_INFO LibDBusCXX
        EXECUTEPROCESS_ARGS
            COMMAND ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
            COMMAND ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} ${greensoftsdk_SOURCE_DIR}/dl
            COMMAND ${MAKE_EXE} libdbus-cpp
        )
endmacro()

greensoftsdk_FindArchiveFile(
    DIRNAME ${PROJECT_SOURCE_DIR}/archives
    PATTERN "greensoft-sdk-*.tar.xz"
)
set(greensoftsdk_tarball_path ${greensoftsdk_FindArchiveFile_result})

include(FetchContent)
FetchContent_Declare(greensoftsdk # Consistency : as FetchContent generated targets are lowercase
    PREFIX  external_dependencies/greensoftsdk
    URL     ${greensoftsdk_tarball_path}
    # todo : replace with remote URL
) 
FetchContent_GetProperties(greensoftsdk)
if(NOT greensoftsdk_POPULATED)
    FetchContent_Populate(greensoftsdk)

    find_program(MAKE_EXE NAMES gmake nmake make)
    message(STATUS "[greensoftsdk] Found MAKE_EXE=[${MAKE_EXE}]")

    greensoftsdk_generateToolchain()
    # set(CMAKE_TOOLCHAIN_FILE greensoftsdk_toolchain.cmake)
    greensoftsdk_generateLibDBusCXX()
    # todo : execute_process => custom_target
    # todo : imported targets
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