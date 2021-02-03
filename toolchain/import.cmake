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
            message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but the following path does not exist [${file_path}]")
        endif()
    endforeach()
endfunction()

if (${USING_GREEN_SDK})

    message(STATUS "[toolchain/import.cmake] Importing GreenSDK ...")
    message(STATUS "[toolchain/import.cmake] custom GreenSDK variable RELOCATED_HOST_DIR=[${RELOCATED_HOST_DIR}]...")

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

    # todo : check import path/target
    # - libdbus
    # - boost
    # - etc.


    set(GreenSDK_sysroot_DIR "${RELOCATED_HOST_DIR}/arm-buildroot-linux-uclibcgnueabi/sysroot/usr" CACHE PATH "GreenSDK toolchain autodetection : SYS_ROOT" FORCE)
    option(GreenSDK_ROOT_PATH_policy "[toolchain/import.cmake] : path to root policy (on=[SDK install DIR], off=[arm-buildroot sysroot/usr])" ON)
    if (GreenSDK_ROOT_PATH_policy)
        set(GreenSDK_ROOTPATH "${GreenSDK_sysroot_DIR}" CACHE PATH "GreenSDK toolchain autodetection : ROOT_PATH" FORCE)
    else()
        set(GreenSDK_ROOTPATH "${RELOCATED_HOST_DIR}" CACHE PATH "GreenSDK toolchain autodetection : ROOT_PATH" FORCE)
    endif()

    # Check toolchain integrity
    AssertFileExists(PATHS
        "${GreenSDK_ROOTPATH}/bin"
        "${GreenSDK_ROOTPATH}/include"
        "${GreenSDK_ROOTPATH}/lib"
        #"${GreenSDK_ROOTPATH}/lib64"
    )
    # GREEN_SDK toolchain directories
    set(GREEN_SDK_BINDIR        "${GreenSDK_ROOTPATH}/bin" CACHE PATH "GreenSDK toolchain autodetection : BINDIR" FORCE)
    set(GREEN_SDK_INCLUDEDIR    "${GreenSDK_ROOTPATH}/include" CACHE PATH "GreenSDK toolchain autodetection : INCLUDEDIR" FORCE)
    set(GREEN_SDK_LIBDIR        "${GreenSDK_ROOTPATH}/lib" CACHE PATH "GreenSDK toolchain autodetection : LIBDIR" FORCE)
    #set(GREEN_SDK_LIB64DIR      "${GreenSDK_ROOTPATH}/lib64" CACHE PATH "GreenSDK toolchain autodetection : LIB64DIR" FORCE)

    # GREEN_SDK_dbusxx_xml2cpp
	set(GREEN_SDK_dbusxx_xml2cpp ${CMAKE_PROGRAM_PATH}/dbusxx-xml2cpp)
	if (NOT (EXISTS ${GREEN_SDK_dbusxx_xml2cpp}))
		message(FATAL_ERROR "[toolchain/import.cmake] GreenSDK toolchain detected, but [${GREEN_SDK_dbusxx_xml2cpp}] does not exists")
	endif()

    set(GREEN_SDK_IMPORTED ON CACHE PATH "GreenSDK toolchain imported after autodetection" FORCE)
else()
    message(WARNING "[toolchain/import.cmake] GreenSDK toolchain was not previously detected")
endif()