# Extract, build, and create targets related to GreenSoftSDK
#   thus generates ${PROJECT_SOURCE_DIR}/generated/GreenSoftSDK directory

include(b4mesh/FindFile)


FindFile(
    DIRNAME ${PROJECT_SOURCE_DIR}/archives
    PATTERN "greensoft-sdk-*.tar.xz"
)
set(greensoftsdk_tarball_path ${FindFile_result})
FindFile(
        DIRNAME ${PROJECT_SOURCE_DIR}/archives
        PATTERN "libdbus-cpp-*.tar.gz"
    )
set(greensoftsdk_libdbuscpp_tarball_path ${FindFile_result})

message(STATUS "[greensoftsdk] ...")
set(greensoftsdk_expected_sha256sum
    "0cd474065448cf4c237f168c8566bc76b18f42da3fece6ef02519b7ec955bf0a"
    CACHE STRING
    "GreenSoftSDK LibDBusC++ : Expected tarball sha256sum result"
)
find_program(MAKE_EXE NAMES gmake nmake make)
include(ExternalProject)

ExternalProject_Add(greensoftsdk
    URL                 ${greensoftsdk_tarball_path}
    URL_HASH            SHA256=${greensoftsdk_expected_sha256sum}

    INSTALL_DIR         ${CMAKE_INSTALL_PREFIX}/dependencies/greensoftsdk/
    CONFIGURE_COMMAND   ""
    BUILD_IN_SOURCE     true    # As Makefile designed to build in-sources ...
    LOG_BUILD           false   # Too many warnings and outputs ...
    #USES_TERMINAL_BUILD true
    BUILD_COMMAND       ${CMAKE_COMMAND} -E echo "[greensoftsdk] Building in <SOURCE_DIR>..."
    COMMAND             ${MAKE_EXE} --silent toolchain
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Built."
    # CMake instance stops here for some reasons ...([greensoftsdk] Built. Build failed.)
    INSTALL_COMMAND     ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installing ..."
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Copying [<SOURCE_DIR>/output] to [<INSTALL_DIR>]"
    #COMMAND            ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/output/host <INSTALL_DIR> # Fail because of unresolved symlink(s) ...
    COMMAND             cp <SOURCE_DIR>/output/host/* <INSTALL_DIR>/ # temporary fix
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installed in <INSTALL_DIR>"
    TEST_COMMAND        ""
    BUILD_BYPRODUCTS
        <INSTALL_DIR>/lib/libdbus-c++-1.so
        #<INSTALL_DIR>/usr/lib/dbus-c++-1.so
        #<INSTALL_DIR>/usr/lib/dbus-c++-1-d.so
        <INSTALL_DIR>/include/dbus-c++-1/
)
ExternalProject_Get_Property(greensoftsdk SOURCE_DIR)
set(greensoftsdk_SOURCE_DIR ${SOURCE_DIR})
ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})

# GreenSoftSDK::DBusCXX

# todo : use SDK's toolchain to build libdbus-cpp

message(STATUS "[greensoftsdk] libdbus-cpp : check tarball sha256 ...")
file(SHA256 ${greensoftsdk_libdbuscpp_tarball_path} greensoftsdk_libdbus-cpp_sha256sum)
set(greensoftsdk_libdbus-cpp_expected_sha256sum
    "6842e99baf73372ae8d047c3b2d79ca2f5d57f900cb436890a9a8ac19930b411"
    CACHE STRING
    "GreenSoftSDK LibDBusC++ : Expected tarball sha256sum result"
)
if (NOT ${greensoftsdk_libdbus-cpp_sha256sum} MATCHES ${greensoftsdk_libdbus-cpp_expected_sha256sum})
    message(FATAL_ERROR
        "[greensoftsdk] libdbus-cpp : sha256sum mismatch for ${greensoftsdk_libdbuscpp_tarball_path}"
        "\n\tExpecting  : [${greensoftsdk_libdbus-cpp_expected_sha256sum}]"
        "\n\tCalculated : [${greensoftsdk_libdbus-cpp_sha256sum}]"
)
endif()

ExternalProject_Add_Step(greensoftsdk libdbus-cpp
    DEPENDEES           build
    DEPENDERS           install
    COMMENT             "[greensoftsdk] libdbus-cpp step ..."
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : create [${greensoftsdk_SOURCE_DIR}/dl/] directory ..."
    COMMAND             ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : copy tarball into .../dl/ ..."
    COMMAND             ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} ${greensoftsdk_SOURCE_DIR}/dl
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Building ..."
    COMMAND             ${MAKE_EXE} libdbus-cpp
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Built ..."
    WORKING_DIRECTORY   ${greensoftsdk_SOURCE_DIR}
)

add_library(DBusCXX SHARED IMPORTED GLOBAL)
set_target_properties(DBusCXX PROPERTIES
    IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
    IMPORTED_LOCATION               ${greensoftsdk_INSTALL_DIR}/lib/libdbus-c++-1.so
    INCLUDE_DIRECTORIES             ${greensoftsdk_INSTALL_DIR}/include/dbus-c++-1/
    # IMPORTED_LOCATION_RELEASE       ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1.so
    # IMPORTED_LOCATION_DEBUG         ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1-d.so
    #INTERFACE_INCLUDE_DIRECTORIES   ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
    #INCLUDE_DIRECTORIES ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
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
