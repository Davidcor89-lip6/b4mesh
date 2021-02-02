# DBusCXX

message(STATUS "[libdbus-cpp] Loading dependency ...")

include(b4mesh/FindFile)
if ((NOT DEFINED GREEN_SDK_IMPORTED) OR (NOT ${GREEN_SDK_IMPORTED}))
    include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/import.cmake)
endif()

find_program(MAKE_EXE NAMES gmake nmake make)
find_program(SHELL_EXE NAMES bash sh)
include(ExternalProject)

if (${USING_GREEN_SDK})
# Import from Green SDK toolchain

    if (NOT ${GREEN_SDK_IMPORTED})
        message(FATAL_ERROR "[libdbus-cpp] Green SDK toolchain detected but GREEN_SDK_IMPORTED=[${GREEN_SDK_IMPORTED}]")
    endif()

    add_library(DBusCXX SHARED IMPORTED GLOBAL)
    set_target_properties(DBusCXX PROPERTIES
        IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
        IMPORTED_LOCATION               ${GREEN_SDK_LIBDIR}/libdbus-c++-1.so
        INTERFACE_INCLUDE_DIRECTORIES   ${GREEN_SDK_INCLUDEDIR}/dbus-c++-1/
        INCLUDE_DIRECTORIES             ${GREEN_SDK_INCLUDEDIR}/dbus-c++-1/
    )
else()
# Autonomous build

    message(WARNING "[libdbus-cpp] This dependency relies on [autoconf], [libdbus-1-dev], [libecore-dev] packages ...")
    # Forked from https://github.com/lib-cpp/dbus-cpp ?

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
