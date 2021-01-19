function(download_Boost)
# Usage :
# find_package(Boost ...) without REQUIRED arg
# if (NOT Boost_FOUND)
#    include(<path/to/>/download_boost.cmake)
#    download_Boost()
# endif()

    if (TARGET Boost)
        message(WARNING "download_Boost : Boost target already defined. Aborting.")
        return()
    endif()
    set(download_Boost_version 1.71.0) # as arg if needed
    message(STATUS "Downloading Boost ${download_Boost_version} ...")
    include(fetchcontent)
    FetchContent_Declare(Boost
        GIT_REPOSITORY      https://github.com/boostorg/boost.git
        GIT_TAG             boost-${download_Boost_version}
        GIT_PROGRESS        TRUE
        FETCHCONTENT_QUIET  OFF
    )
    FetchContent_MakeAvailable(Boost)
    FetchContent_GetProperties(Boost)

    # todo : replace with ExternalProject_Add
    #   as boost repo using git submodules for its own dependencies
endfunction()