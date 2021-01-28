if (NOT (TARGET greensoftsdk))
    message(FATAL_ERROR "[libdbus-cpp] : requires greensoftsdk")
endif()

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

ExternalProject_Add_Step(greensoftsdk add_libdbus-cpp
    DEPENDEES           configure
    DEPENDERS           build
    COMMENT             "[greensoftsdk] libdbus-cpp step ..."
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : create [${greensoftsdk_SOURCE_DIR}/dl/] directory ..."
    COMMAND             ${CMAKE_COMMAND} -E make_directory ${greensoftsdk_SOURCE_DIR}/dl/
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : copy tarball into .../dl/ ..."
    COMMAND             ${CMAKE_COMMAND} -E copy ${greensoftsdk_libdbuscpp_tarball_path} ${greensoftsdk_SOURCE_DIR}/dl
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Building ..."
    COMMAND             ${MAKE_EXE}  --silent libdbus-cpp
    COMMAND             ${CMAKE_COMMAND} -E echo "[greensoftsdk] libdbus-cpp step : Built ..."
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
    INCLUDE_DIRECTORIES             ${greensoftsdk_INSTALL_DIR}/greensoftsdk/include/dbus-c++-1/
    # IMPORTED_LOCATION_RELEASE       ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1.so
    # IMPORTED_LOCATION_DEBUG         ${greensoftsdk_INSTALL_DIR}/usr/lib/dbus-c++-1-d.so
    #INTERFACE_INCLUDE_DIRECTORIES   ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
    #INCLUDE_DIRECTORIES ${greensoftsdk_INSTALL_DIR}/usr/include/dbus-c++-1
)
add_dependencies(DBusCXX
    greensoftsdk
    greensoftsdk-libdbus-cpp
)
target_link_libraries(DBusCXX INTERFACE greensoftsdk)
add_library(GreenSoftSDK::DBusCXX ALIAS DBusCXX)