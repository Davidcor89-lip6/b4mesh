cmake_minimum_required(VERSION 3.12)

function (get_nodes_filestream_path SOURCES_PATH)
    if (NOT DEFINED SOURCES_PATH)
        message(FATAL_ERROR "get_nodes_filestream_path : `SOURCES_PATH` is a mandatory argument")
    endif()

    set(filestream_path_regex ".*LIVEBLOCK_FILE\ *\=?\ *.*\"(.+)\"")
    set(target_file ${SOURCES_PATH}/include/configs.hpp)
    file(STRINGS
        ${target_file}
        result
        REGEX ${filestream_path_regex}
    )
    string(REGEX REPLACE ${filestream_path_regex}
       "\\1" result
       "${result}")
    if (result STREQUAL "")
        message(FATAL_ERROR "get_nodes_filestream_path : Cannot find LIVEBLOCK_FILE compile-time constant in ${target_file}")
    endif()
    set (nodes_filestream_path ${result} PARENT_SCOPE)
endfunction()
