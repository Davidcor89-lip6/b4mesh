cmake_minimum_required(VERSION 3.12)

function (send_to_remote_machines)
#  file_or_directory remote_path

    set(options VERBOSE)
    set(oneValueArgs PATH SSHPASS_USER SSH_USER)
    set(multiValueArgs DESTINATIONS FILES DIRECTORIES)
    cmake_parse_arguments(SEND_TO_REMOTE_MACHINES
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (NOT DEFINED SEND_TO_REMOTE_MACHINES_PATH)
        message(FATAL_ERROR  "send_to_remote_machines : missing mandatory argument `PATH`")
    endif()

    if (DEFINED SEND_TO_REMOTE_MACHINES_SSHPASS_USER)
        set(command_prefix "sshpass -p '${SEND_TO_REMOTE_MACHINES_SSHPASS_USER}'")
    else()
        set(command_prefix "")
    endif()

    if (NOT DEFINED SEND_TO_REMOTE_MACHINES_SSH_USER)
        message(FATAL_ERROR "send_to_remote_machines : missing or invalid mandatory argument `SSH_USER`")
    endif()

    foreach(destination IN LISTS SEND_TO_REMOTE_MACHINES_DESTINATIONS)
        if (SEND_TO_REMOTE_MACHINES_VERBOSE)
            message(STATUS "send_to_remote_machines : destination : ${destination}")
        endif()

        set(mkdir_command "${command_prefix} ssh ${SEND_TO_REMOTE_MACHINES_SSH_USER}@${destination} \"mkdir -p ${SEND_TO_REMOTE_MACHINES_PATH}\"")
        if (DEFINED SEND_TO_REMOTE_MACHINES_VERBOSE)
            message(STATUS "send_to_remote_machines : command : ${mkdir_command}")
        endif()

        # mkdir on remote machine is allowed to fail (destination directory may already exists)
        execute_process(COMMAND
            bash "-c" "${mkdir_command}"
            #COMMAND_ERROR_IS_FATAL ANY
            #ECHO_OUTPUT_VARIABLE
            #ECHO_ERROR_VARIABLE
        )
        # if (NOT (result STREQUAL "0"))
        #     message(FATAL_ERROR "send_to_remote_machines : error on command : ${result}")
        # endif()

        foreach(file IN LISTS SEND_TO_REMOTE_MACHINES_FILES)
            set(send_file_command "${command_prefix} scp ${file} ${SEND_TO_REMOTE_MACHINES_SSH_USER}@${destination}:${SEND_TO_REMOTE_MACHINES_PATH}")
            if (DEFINED SEND_TO_REMOTE_MACHINES_VERBOSE)
                message(STATUS "send_to_remote_machines : command : ${send_file_command}")
            endif()
            execute_process(COMMAND
                bash "-c" "${send_file_command}"
                COMMAND_ERROR_IS_FATAL ANY
                RESULT_VARIABLE     result
                ERROR_VARIABLE      error
                #ECHO_OUTPUT_VARIABLE
                #ECHO_ERROR_VARIABLE
            )
            if (NOT (result STREQUAL "0"))
                message(FATAL_ERROR " send_to_remote_machines :\n"
                    " - error : ${error}"
                    " - return : ${result}"
                )
            endif()
        endforeach()
        foreach(directory IN LISTS SEND_TO_REMOTE_MACHINES_DIRECTORIES)
            set(send_dir_command "${command_prefix} scp -r ${file} ${SEND_TO_REMOTE_MACHINES_SSH_USER}@${destination}:${SEND_TO_REMOTE_MACHINES_PATH}")
            if (DEFINED SEND_TO_REMOTE_MACHINES_VERBOSE)
                message(STATUS "send_to_remote_machines : command : ${send_dir_command}")
            endif()
            execute_process(COMMAND
                bash "-c" "${send_dir_command}"
                COMMAND_ERROR_IS_FATAL ANY
                RESULT_VARIABLE     result
                ERROR_VARIABLE      error
                #ECHO_OUTPUT_VARIABLE
                #ECHO_ERROR_VARIABLE
            )
            if (NOT (result STREQUAL "0"))
                message(FATAL_ERROR " send_to_remote_machines :\n"
                    " - error : ${error}"
                    " - return : ${result}"
                )
            endif()
        endforeach()
    endforeach()

endfunction()
