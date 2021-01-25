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

greensoftsdk_FindArchiveFile(
    DIRNAME ${PROJECT_SOURCE_DIR}/archives
    PATTERN "greensoft-sdk-*.tar.xz"
)
set(greensoftsdk_tarball_path ${greensoftsdk_FindArchiveFile_result})
greensoftsdk_FindArchiveFile(
        DIRNAME ${PROJECT_SOURCE_DIR}/archives
        PATTERN "libdbus-cpp-*.tar.gz"
    )
set(greensoftsdk_libdbuscpp_tarball_path ${greensoftsdk_FindArchiveFile_result})

message(STATUS "[greensoftsdk] ...")
find_program(MAKE_EXE NAMES gmake nmake make)
include(ExternalProject)
ExternalProject_Add(greensoftsdk
    URL                 ${greensoftsdk_tarball_path}
    URL_HASH            SHA256=0cd474065448cf4c237f168c8566bc76b18f42da3fece6ef02519b7ec955bf0a

    CONFIGURE_COMMAND   ""
    BUILD_IN_SOURCE     true    # As Makefile designed to build in-sources ...
    LOG_BUILD           false   # Too many warnings and outputs ...
    BUILD_COMMAND       ${CMAKE_COMMAND} -E echo "[greensoftsdk] Building in <SOURCE_DIR>..."
    COMMAND             ${MAKE_EXE} toolchain
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Built."
    INSTALL_COMMAND     ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installing ..."
    COMMAND             ${CMAKE_COMMAND} -E copy <SOURCE_DIR> <INSTALL_DIR>
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installed in <INSTALL_DIR>."
    TEST_COMMAND        ""
    BUILD_BYPRODUCTS
        <INSTALL_DIR>/usr/lib/dbus-c++-1.so
        <INSTALL_DIR>/usr/lib/dbus-c++-1-d.so
        <INSTALL_DIR>/usr/include/dbus-c++-1
        # <SOURCE_DIR>/usr/lib/dbus-c++-1.so
        # <SOURCE_DIR>/usr/lib/dbus-c++-1-d.so
)
ExternalProject_Get_Property(greensoftsdk SOURCE_DIR)
set(greensoftsdk_SOURCE_DIR ${SOURCE_DIR})
ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})

message(STATUS "greensoftsdk_SOURCE_DIR=[${greensoftsdk_SOURCE_DIR}]")
message(STATUS "greensoftsdk_INSTALL_DIR=[${greensoftsdk_INSTALL_DIR}]")

ExternalProject_Add_Step(greensoftsdk libdbus-cpp
    DEPENDEES           configure build
    DEPENDERS           install
    COMMENT             "[greensoftsdk] libdbus-cpp step ..."
    # DEPENDEES           greensoftsdk-configure
    #                     greensoftsdk-build
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : check tarball sha256 ..."
    COMMAND             if [ (${CMAKE_COMMAND} -E sha256sum ${greensoftsdk_libdbuscpp_tarball_path}) -ne "6842e99baf73372ae8d047c3b2d79ca2f5d57f900cb436890a9a8ac19930b411" ] \; then ${CMAKE_COMMAND} -E false\; fi
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : create [${greensoftsdk_SOURCE_DIR}/dl/] directory ..."
    COMMAND             ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : copy tarball into .../dl/ ..."
    COMMAND             ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} ${greensoftsdk_SOURCE_DIR}/dl
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Building ..."
    COMMAND             ${MAKE_EXE} libdbus-cpp
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Built ..."
    WORKING_DIRECTORY   ${greensoftsdk_SOURCE_DIR}
)

# GreenSoftSDK::DBusCXX

message(STATUS ">>>>>>>>>  ${greensoftsdk_SOURCE_DIR}/usr/lib/dbus-c++-1.so")
message(STATUS ">>>>>>>>>  ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1.so")

add_library(DBusCXX SHARED IMPORTED GLOBAL)
set_target_properties(DBusCXX PROPERTIES
    IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
    IMPORTED_LOCATION_RELEASE       ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1.so
    IMPORTED_LOCATION_DEBUG         ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1-d.so
    #INTERFACE_INCLUDE_DIRECTORIES   ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
    INCLUDE_DIRECTORIES ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
)
add_dependencies(DBusCXX greensoftsdk greensoftsdk-libdbus-cpp)
target_link_libraries(DBusCXX INTERFACE greensoftsdk)
add_library(GreenSoftSDK::DBusCXX ALIAS DBusCXX)

# Toolchains
# add_custom_target(greenSoftSDK_toolchain_host)
# add_custom_target(greenSoftSDK_toolchain_local)

# Generates toolchain files for -DCMAKE_TOOLCHAIN_FILE
# Force toolchain
#   SET(CMAKE_TOOLCHAIN_FILE  "${GreenSoftSDK_toolchain_path}" CACHE INTERNAL "CMAKE_TOOLCHAIN_FILE")
