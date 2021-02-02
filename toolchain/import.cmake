# Complete/Add unspecified elements from GreenSDK toolchain

function(AssertFileExists)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PATHS)
    cmake_parse_arguments(AssertFileExists
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    foreach(file_path IN LISTS AssertFileExists_PATHS)
        if (NOT (EXISTS ${file_path}))
            message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but toolchain does not set [RELOCATED_HOST_DIR]")
        endif()
    endforeach()
endfunction()

if (${USING_GREEN_SDK})

    message(STATUS "[toolchain/import.cmake] Importing GreenSDK ...")

    if (NOT DEFINED CMAKE_CXX_COMPILER)
        message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but no CMAKE_CXX_COMPILER specified")
    endif()
    message(STATUS "[toolchain/import.cmake] CMAKE_CXX_COMPILER=[${CMAKE_CXX_COMPILER}]")

    if (NOT DEFINED RELOCATED_HOST_DIR)
        message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but toolchain does not set [RELOCATED_HOST_DIR]")
    endif()
    if (NOT DEFINED CMAKE_PROGRAM_PATH)
		message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but toolchain does not set [CMAKE_PROGRAM_PATH]")
	endif()

    # Check toolchain integrity
    AssertFileExists(PATHS
        "${RELOCATED_HOST_DIR}/bin"
        "${RELOCATED_HOST_DIR}/include"
        "${RELOCATED_HOST_DIR}/lib"
        "${RELOCATED_HOST_DIR}/lib64"
    )
    # GREEN_SDK toolchain directories
    set(GREEN_SDK_BINDIR "${RELOCATED_HOST_DIR}/BIN" CACHE PATH "GreenSDK toolchain autodetection : BINDIR" FORCE)
    set(GREEN_SDK_INCLUDEDIR "${RELOCATED_HOST_DIR}/lib" CACHE PATH "GreenSDK toolchain autodetection : INCLUDEDIRDIR" FORCE)
    set(GREEN_SDK_LIBDIR "${RELOCATED_HOST_DIR}/lib" CACHE PATH "GreenSDK toolchain autodetection : LIBDIR" FORCE)
    set(GREEN_SDK_LIB64DIR "${RELOCATED_HOST_DIR}/lib64" CACHE PATH "GreenSDK toolchain autodetection : LIB64DIR" FORCE)

    # GREEN_SDK_dbusxx_xml2cpp
	set(GREEN_SDK_dbusxx_xml2cpp ${CMAKE_PROGRAM_PATH}/dbusxx-xml2cpp)
	if (NOT (EXISTS ${GREEN_SDK_dbusxx_xml2cpp}))
		message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but [${GREEN_SDK_dbusxx_xml2cpp}] does not exists")
	endif()

    set(GREEN_SDK_IMPORTED ON CACHE PATH "GreenSDK toolchain imported after autodetection" FORCE)
else()
    message(WARNING "[toolchain/import.cmake] GreenSDK toolchain was not previously detected")
endif()