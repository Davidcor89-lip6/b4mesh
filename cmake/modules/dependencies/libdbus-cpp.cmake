# DBusCXX

include(b4mesh/FindFile)

message(STATUS "[libdbus-cpp] Loading dependency ...")

FindFile(
    DIRNAME ${PROJECT_SOURCE_DIR}/toolchain/archives
    PATTERN "libdbus-cpp-*.tar.gz"
)
set(greensoftsdk_libdbuscpp_tarball_path ${FindFile_result})

message(STATUS "[libdbus-cpp] check tarball sha256 ...")
file(SHA256 ${greensoftsdk_libdbuscpp_tarball_path} greensoftsdk_libdbus-cpp_sha256sum)
set(greensoftsdk_libdbus-cpp_expected_sha256sum
    "6842e99baf73372ae8d047c3b2d79ca2f5d57f900cb436890a9a8ac19930b411"
    CACHE STRING
    "GreenSoftSDK LibDBusC++ : Expected tarball sha256sum result"
)

find_program(MAKE_EXE NAMES gmake nmake make)
find_program(SHELL_EXE NAMES bash sh)
include(ExternalProject)

if (${USE_GREEN_SDK})
    if (NOT (TARGET greensoftsdk))
        message(FATAL_ERROR "[libdbus-cpp] : requires greensoftsdk target when [USE_GREEN_SDK] is [on]")
    endif()

    if (NOT ${greensoftsdk_libdbus-cpp_sha256sum} MATCHES ${greensoftsdk_libdbus-cpp_expected_sha256sum})
    message(FATAL_ERROR
        "[greensoftsdk] libdbus-cpp : sha256sum mismatch for ${greensoftsdk_libdbuscpp_tarball_path}"
        "\n\tExpecting  : [${greensoftsdk_libdbus-cpp_expected_sha256sum}]"
        "\n\tCalculated : [${greensoftsdk_libdbus-cpp_sha256sum}]"
    )
    endif()

    ExternalProject_Get_Property(greensoftsdk SOURCE_DIR)
    set(greensoftsdk_SOURCE_DIR ${SOURCE_DIR})
    ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
    set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})

    # todo : BR2_PACKAGE_LIBDBUS_CPP=y

    ExternalProject_Add_Step(greensoftsdk add_libdbus-cpp
        DEPENDEES           configure
        DEPENDERS           build
        COMMENT             "[greensoftsdk] libdbus-cpp step ..."
        COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : create [${greensoftsdk_SOURCE_DIR}/dl/] directory ..."
        COMMAND             ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
        COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : copy tarball into .../dl/ ..."
        COMMAND             ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} ${greensoftsdk_SOURCE_DIR}/dl
        # COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Building ..."
        # COMMAND             ${MAKE_EXE}  --silent libdbus-cpp # useless ?
        # COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Built ..."
        WORKING_DIRECTORY   ${greensoftsdk_SOURCE_DIR}
    )

    if (NOT DEFINED greensoftsdk_INSTALL_DIR)
        ExternalProject_Get_Property(greensoftsdk INSTALL_DIR)
        set(greensoftsdk_INSTALL_DIR ${INSTALL_DIR})
    endif()

    add_library(DBusCXX UNKNOWN IMPORTED GLOBAL)
    set_target_properties(DBusCXX PROPERTIES
        IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
        IMPORTED_LOCATION               ${greensoftsdk_INSTALL_DIR}/greensoftsdk/lib/libdbus-c++-1.so
        INTERFACE_INCLUDE_DIRECTORIES   ${greensoftsdk_INSTALL_DIR}/greensoftsdk/include/dbus-c++-1/
        # IMPORTED_LOCATION_RELEASE       ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1.so
        # IMPORTED_LOCATION_DEBUG         ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1-d.so
    )
    add_dependencies(DBusCXX
        greensoftsdk
        greensoftsdk-libdbus-cpp
    )
    target_link_libraries(DBusCXX INTERFACE greensoftsdk)
    add_library(GreenSoftSDK::DBusCXX ALIAS DBusCXX)
else()
    message(WARNING "[dependencies::libdbus-cpp] This dependency relies on [autoconf], [libdbus-1-dev], [libecore-dev] packages ...")

    # Forked from https://github.com/lib-cpp/dbus-cpp ?

    ExternalProject_Add(libdbuscpp_import
        URL                 ${greensoftsdk_libdbuscpp_tarball_path}
        URL_HASH            SHA256=${greensoftsdk_libdbus-cpp_expected_sha256sum}

        INSTALL_DIR         ${CMAKE_INSTALL_PREFIX}/libdbuscpp
        BUILD_IN_SOURCE     true    # As Makefile designed to build in-sources ...
        LOG_BUILD           false   # Still tons of warnings and outputs ...

        CONFIGURE_COMMAND   ${SHELL_EXE} <SOURCE_DIR>/autogen.sh --silent --prefix=<INSTALL_DIR>
        BUILD_COMMAND       ${CMAKE_COMMAND} -E echo "[dependencies::libdbus-cpp] Building in <SOURCE_DIR>..."
        COMMAND             ${MAKE_EXE} --silent --enable-debug
        COMMAND             ${CMAKE_COMMAND} -E echo "[dependencies::libdbus-cpp] Built."
        INSTALL_COMMAND     ${CMAKE_COMMAND} -E echo "[dependencies::libdbus-cpp] Installing ..."
        COMMAND             ${MAKE_EXE} --silent install
        COMMAND             ${CMAKE_COMMAND} -E echo "[dependencies::libdbus-cpp] Installed."
        TEST_COMMAND        ""

        BUILD_BYPRODUCTS
            <INSTALL_DIR>/include/dbus-c++-1/
            <INSTALL_DIR>/lib/libdbus-c++-1.a
            <INSTALL_DIR>/lib/libdbus-c++-1.so
            <INSTALL_DIR>/lib/libdbus-c++-1.so.0.0.0
            <INSTALL_DIR>/lib/libdbus-c++-ecore-1.la
            <INSTALL_DIR>/lib/libdbus-c++-ecore-1.so.0
            <INSTALL_DIR>/lib/libdbus-c++-glib-1.a
            <INSTALL_DIR>/lib/libdbus-c++-glib-1.so
            <INSTALL_DIR>/lib/libdbus-c++-glib-1.so.0.0.0
            <INSTALL_DIR>/lib/libdbus-c++-1.la
            <INSTALL_DIR>/lib/libdbus-c++-1.so.0
            <INSTALL_DIR>/lib/libdbus-c++-ecore-1.a
            <INSTALL_DIR>/lib/libdbus-c++-ecore-1.so
            <INSTALL_DIR>/lib/libdbus-c++-ecore-1.so.0.0.0
            <INSTALL_DIR>/lib/libdbus-c++-glib-1.la
            <INSTALL_DIR>/lib/libdbus-c++-glib-1.so.0
    )

    ExternalProject_Get_Property(libdbuscpp_import INSTALL_DIR)
    set(libdbuscpp_import_INSTALL_DIR ${INSTALL_DIR})

    add_library(DBusCXX INTERFACE)
    target_include_directories(DBusCXX INTERFACE
        ${libdbuscpp_import_INSTALL_DIR}/include/dbus-c++-1/
    )
    add_library(DBusCXX_libs UNKNOWN IMPORTED GLOBAL)
    set_target_properties(DBusCXX_libs PROPERTIES
        IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
        IMPORTED_LOCATION               ${libdbuscpp_import_INSTALL_DIR}/lib/libdbus-c++-1.so
        # INTERFACE_INCLUDE_DIRECTORIES   ${libdbuscpp_import_INSTALL_DIR}/include/dbus-c++-1/
    )
    add_dependencies(DBusCXX_libs libdbuscpp_import)
    target_link_libraries(DBusCXX INTERFACE DBusCXX_libs)
endif()
