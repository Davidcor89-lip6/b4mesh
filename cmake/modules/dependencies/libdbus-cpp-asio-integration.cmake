# DBusCXX

message(STATUS "[libdbus-cpp-asio-integration] Loading dependency ...")

include(b4mesh/FindFile)
if ((NOT DEFINED GREEN_SDK_IMPORTED) OR (NOT ${GREEN_SDK_IMPORTED}))
    include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/import.cmake)
endif()

if (NOT TARGET DBusCXX)
    include(${CMAKE_SOURCE_DIR}/cmake/modules/dependencies/libdbus-cpp.cmake)
endif()

if (${USING_GREEN_SDK})
# Import from Green SDK toolchain
# or use /toolchain/greensoftsdk/lib/cmake/DBus1/DBus1Config.cmake

    if (NOT ${GREEN_SDK_IMPORTED})
        message(FATAL_ERROR "[libdbus-cpp-asio-integration] Green SDK toolchain detected but GREEN_SDK_IMPORTED=[${GREEN_SDK_IMPORTED}]")
    endif()

    add_library(DBusCXX_asio_integration SHARED IMPORTED GLOBAL)
    set_target_properties(DBusCXX_asio_integration PROPERTIES
        #IMPORTED_CONFIGURATIONS         "RELEASE;DEBUG"
        IMPORTED_LOCATION               ${GREEN_SDK_LIBDIR}/libdbus-c++-asio-1.so
        INTERFACE_INCLUDE_DIRECTORIES   ${GREEN_SDK_INCLUDEDIR}/dbus-c++-1
        INCLUDE_DIRECTORIES             ${GREEN_SDK_INCLUDEDIR}/dbus-c++-1
    )
    add_dependencies(DBusCXX_asio_integration
        public
            DBusCXX
    )
else()
    message(FATAL_ERROR "[libdbus-cpp-asio-integration] : depends on GreenSDK")
endif()

# Check library integrity
get_target_property(Check_DBusCXX_INCLUDEDIR DBusCXX INTERFACE_INCLUDE_DIRECTORIES)
if (NOT EXISTS "${Check_DBusCXX_INCLUDEDIR}/dbus-c++/asio-integration.h")
    message(FATAL_ERROR "[libdbus-cpp] Invalid include path, main header does not exists [${Check_DBusCXX_INCLUDEDIR}/dbus-c++/asio-integration.h]")
endif()