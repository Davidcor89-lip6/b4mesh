# Extract, build, and create targets related to GreenSoftSDK

if ($ENV{USER} STREQUAL "")
    message(FATAL_ERROR "[greensoftsdk] USER not set in ENV")
endif()

include(b4mesh/FindFile)

FindFile(
    DIRNAME ${CMAKE_CURRENT_SOURCE_DIR}/archives
    PATTERN "greensoft-sdk-*.tar.xz"
)
set(greensoftsdk_tarball_path ${FindFile_result})

message(STATUS "[greensoftsdk] ...")
set(greensoftsdk_expected_sha256sum
    "4df49ed8637e3fd1387c8622b0e2cb33adff9c849c2fcf0b3cd4ff33185c4e36"
    CACHE STRING
    "GreenSoftSDK LibDBusC++ : Expected tarball sha256sum result"
)
find_program(MAKE_EXE NAMES gmake nmake make)
find_program(SHELL_EXE NAMES bash sh) # todo : replace shell usage by direct CMake script invocation
include(ExternalProject)

set(greensoftsdk_package_name arm-buildroot-linux-uclibcgnueabi_sdk-buildroot)
ExternalProject_Add(greensoftsdk
    # NB : all steps, including extras are :
    #         patch_dot_config_file
    #         configure
    #         add_libdbus-cpp
    #         build
    #         install
    #         custom_install
    #         install_cmake_toolchain_descriptor

    URL                 ${greensoftsdk_tarball_path}
    URL_HASH            SHA256=${greensoftsdk_expected_sha256sum}

    INSTALL_DIR         ${CMAKE_INSTALL_PREFIX}/toolchain
    CONFIGURE_COMMAND   ${CMAKE_COMMAND} -E echo "[greensoftsdk] Configuration ..."
    COMMAND             ${MAKE_EXE} olddefconfig

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
ExternalProject_Get_Property(greensoftsdk SOURCE_DIR)
set(greensoftsdk_SOURCE_DIR ${SOURCE_DIR})
ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})

FindFile(
    DIRNAME ${CMAKE_CURRENT_SOURCE_DIR}/archives
    PATTERN "libdbus-cpp-*.tar.gz"
)
set(greensoftsdk_libdbuscpp_tarball_path ${FindFile_result})

file(SHA256 ${greensoftsdk_libdbuscpp_tarball_path} greensoftsdk_libdbus-cpp_sha256sum)
set(greensoftsdk_libdbus-cpp_expected_sha256sum
    "4ff9ab1f894b45d3a95705147e16a5565aa9f1859ca4cd20c202e4a9bee68c44"
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

### Subsetp : "[greensoftsdk] patching [.config] file step ..."

set(dot_config_to_add_lines
    BR2_PACKAGE_BOOST=y
    BR2_PACKAGE_BOOST_SYSTEM=y
    BR2_PACKAGE_LIBDBUS_CPP=y
    BR2_TOOLCHAIN_BUILDROOT_WCHAR=y
    BR2_PACKAGE_BOOST_RANDOM=y
)
set(dot_config_to_remove_lines
# WIP avec Green : ces deux options sont inutiles pour notre cas
# https://github.com/Davidcor89-lip6/b4mesh/issues/44#issuecomment-831288991

    ^BR2_TARGET_UBOOT
    ^BR2_ROOTFS_POST_IMAGE_SCRIPT
)

# Portable List conversion : "A;B" -> "A$<SEMICOLON>B"
set(dot_config_to_add_lines_to_str "${dot_config_to_add_lines}")
set(dot_config_to_remove_lines_to_str "${dot_config_to_remove_lines}")
string(REPLACE ";" "$<SEMICOLON>" dot_config_to_add_lines_to_str "${dot_config_to_add_lines_to_str}")
string(REPLACE ";" "$<SEMICOLON>" dot_config_to_remove_lines_to_str "${dot_config_to_remove_lines_to_str}")

ExternalProject_Add_Step(greensoftsdk patch_dot_config_file
    DEPENDEES           update
    DEPENDERS           configure
    
    COMMENT             "[greensoftsdk] patching [.config] file step ..."
    COMMAND             ${CMAKE_COMMAND}  
                            -DFileIO_DIRECT_CALL:BOOL=ON
                            -DFileIO_PATH:PATH=<SOURCE_DIR>/.config
                            -DFileIO_TO_ADD=${dot_config_to_add_lines_to_str}
                            -DFileIO_TO_REMOVE_PATTERNS=${dot_config_to_remove_lines_to_str}
                            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FileIO.cmake

    WORKING_DIRECTORY   <SOURCE_DIR>
    BYPRODUCTS          <SOURCE_DIR>/.config
    ALWAYS              true
)

ExternalProject_Add_Step(greensoftsdk add_libdbus-cpp
    DEPENDEES           configure
    DEPENDERS           build
    COMMENT             "[greensoftsdk] libdbus-cpp step ..."
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : create [<SOURCE_DIR>/dl/] directory ..."
    COMMAND             ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : copy tarball [${greensoftsdk_libdbuscpp_tarball_path}] into [<SOURCE_DIR>/dl/]"
    COMMAND             ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} <SOURCE_DIR>/dl
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Building ..."
    COMMAND             ${MAKE_EXE}  --silent libdbus-cpp
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Built ..."
    WORKING_DIRECTORY   <SOURCE_DIR>
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
    COMMAND             ${SHELL_EXE} <INSTALL_DIR>/greensoftsdk/relocate-sdk.sh # todo : as cmake script
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] Installed in <INSTALL_DIR>"
)

set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR} PARENT_SCOPE)
SET(greensoftsdk_toolchain_descriptor "${CMAKE_CURRENT_SOURCE_DIR}/greensoftsdk.toolchain.cmake"
	CACHE INTERNAL "(source) greensoftsdk cmake toolchain descriptor path"
)
SET(greensoftsdk_toolchain_descriptor_installed "${greensoftsdk_INSTALL_DIR}/greensoftsdk/greensoftsdk.toolchain.cmake"
	CACHE INTERNAL "(installed) greensoftsdk cmake toolchain descriptor path"
)
