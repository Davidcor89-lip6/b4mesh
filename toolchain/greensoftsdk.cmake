# Extract, build, and create targets related to GreenSoftSDK

include(b4mesh/FindFile)

FindFile(
    DIRNAME ${CMAKE_CURRENT_SOURCE_DIR}/archives
    PATTERN "greensoft-sdk-*.tar.xz"
)
set(greensoftsdk_tarball_path ${FindFile_result})
FindFile(
        DIRNAME ${CMAKE_CURRENT_SOURCE_DIR}/archives
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
find_program(SHELL_EXE NAMES bash sh)
include(ExternalProject)

set(greensoftsdk_package_name arm-buildroot-linux-uclibcgnueabi_sdk-buildroot)
ExternalProject_Add(greensoftsdk
    # NB : extra steps
    #         from libdbus-cpp.make   : add_libdbus-cpp (between configure and build)
    #         from greensoftsdk.cmake : custom_install (after install)
    #         from greensoftsdk.cmake : install_cmake_toolchain_descriptor (after custom_install)

    URL                 ${greensoftsdk_tarball_path}
    URL_HASH            SHA256=${greensoftsdk_expected_sha256sum}

    INSTALL_DIR         ${CMAKE_INSTALL_PREFIX}/toolchain
    CONFIGURE_COMMAND   "" # todo : patch config file for boost config ...
    BUILD_IN_SOURCE     true    # As Makefile designed to build in-sources ...
    LOG_BUILD           false   # Still tons of warnings and outputs ...
    BUILD_COMMAND       ${CMAKE_COMMAND} -E echo "[greensoftsdk] Building SDK in <SOURCE_DIR>..."
    COMMAND             ${MAKE_EXE} --silent sdk #toolchain
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Built."
    INSTALL_COMMAND     ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installing ..."
    TEST_COMMAND        ""

    BUILD_BYPRODUCTS
        <INSTALL_DIR>/greensoftsdk
        <INSTALL_DIR>/greensoftsdk/greensoftsdk.toolchain.cmake
        <INSTALL_DIR>/greensoftsdk/lib/libdbus-c++-1.so
)

ExternalProject_Add_Step(greensoftsdk custom_install
    DEPENDEES           install
    COMMENT             "[greensoftsdk] custom_install step ..."
    WORKING_DIRECTORY   <INSTALL_DIR>
    
    COMMAND             ${CMAKE_COMMAND} -E rm -rf <INSTALL_DIR>/${greensoftsdk_package_name}
    COMMAND             ${CMAKE_COMMAND} -E rm -rf <INSTALL_DIR>/greensoftsdk
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Extracting [<SOURCE_DIR>/output/images/${greensoftsdk_package_name}.tar.gz]"
    COMMAND             ${CMAKE_COMMAND} -E tar x <SOURCE_DIR>/output/images/${greensoftsdk_package_name}.tar.gz
    COMMAND             ${CMAKE_COMMAND} -E rename <INSTALL_DIR>/${greensoftsdk_package_name} <INSTALL_DIR>/greensoftsdk
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Running [relocate-sdk.sh] script ..."
    COMMAND             ${SHELL_EXE} <INSTALL_DIR>/greensoftsdk/relocate-sdk.sh
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installed in <INSTALL_DIR>"
)
ExternalProject_Add_Step(greensoftsdk install_cmake_toolchain_descriptor
    DEPENDEES           custom_install
    COMMENT             "[greensoftsdk] install_cmake_toolchain_descriptor step ..."
    WORKING_DIRECTORY   <INSTALL_DIR>

    COMMAND             ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.toolchain.cmake <INSTALL_DIR>
)

ExternalProject_Get_Property(greensoftsdk SOURCE_DIR)
set(greensoftsdk_SOURCE_DIR ${SOURCE_DIR})
ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})
set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR} PARENT_SCOPE)
SET(greensoftsdk_toolchain_descriptor "${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.toolchain.cmake"
	CACHE INTERNAL "(source) greensoftsdk cmake toolchain descriptor path"
)
SET(greensoftsdk_toolchain_descriptor_installed "${greensoftsdk_INSTALL_DIR}/greensoftsdk/greensoftsdk.toolchain.cmake"
	CACHE INTERNAL "(installed) greensoftsdk cmake toolchain descriptor path"
)
