function(FindFile)
# Set `FINDFILE_result`

    set(options)
    set(oneValueArgs DIRNAME PATTERN)
    set(multiValueArgs)
    cmake_parse_arguments(FINDFILE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "[FindFile] Scanning [${FINDFILE_DIRNAME}] for [${FINDFILE_PATTERN}] ...")
    file(GLOB filesnames
        LIST_DIRECTORIES false
        # CONFIGURE_DEPENDS
        ${FINDFILE_DIRNAME}/${FINDFILE_PATTERN}
    )

    list(LENGTH filesnames GreenSoftSDK_filename_matches)
    if (NOT(${GreenSoftSDK_filename_matches} EQUAL 1))
        message(FATAL_ERROR
            "[FindFile] Wrong matches quantity (expected exactly one) :"
            "${filesnames}"
        )
    endif()
    list(GET filesnames 0 filesnames)

    message(STATUS "[FindFile] Found    [${filesnames}]")
    set(FindFile_result ${filesnames} PARENT_SCOPE)
endfunction()