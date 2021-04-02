cmake_minimum_required(VERSION 3.12)

function (initialize_remote_FS)

    set(options VERBOSE)
    set(oneValueArgs PATH SSHPASS_USER SSH_USER ROOT_PASSWORD)
    set(multiValueArgs DESTINATIONS)
    cmake_parse_arguments(INITIALIZE_REMOTE_FS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (NOT DEFINED INITIALIZE_REMOTE_FS_PATH)
        message(FATAL_ERROR  "initialize_remote_FS : missing mandatory argument `PATH`")
    endif()

    if (DEFINED INITIALIZE_REMOTE_FS_SSHPASS_USER)
        set(command_prefix "sshpass -p '${INITIALIZE_REMOTE_FS_SSHPASS_USER}'")
    else()
        set(command_prefix "")
    endif()

    if (NOT DEFINED INITIALIZE_REMOTE_FS_SSH_USER)
        message(FATAL_ERROR "initialize_remote_FS : missing or invalid mandatory argument `SSH_USER`")
    endif()
    
    foreach(destination IN LISTS INITIALIZE_REMOTE_FS_DESTINATIONS)

        if (INITIALIZE_REMOTE_FS_VERBOSE)
            message(STATUS "initialize_remote_FS : destination : ${destination}")
        endif()

        set(remote_command
            "${command_prefix}  \
            ssh ${INITIALIZE_REMOTE_FS_SSH_USER}@${destination} \"          \
                    (mkdir -p ${INITIALIZE_REMOTE_FS_PATH} || true) &&      \
                    echo '${INITIALIZE_REMOTE_FS_ROOT_PASSWORD}' | sudo -S  \
                        chgrp -R www-data ${INITIALIZE_REMOTE_FS_PATH} &&   \
                    echo '${INITIALIZE_REMOTE_FS_ROOT_PASSWORD}' | sudo -S  \
                        chmod -R g+rX ${INITIALIZE_REMOTE_FS_PATH}          \
            \"")
        if (DEFINED INITIALIZE_REMOTE_FS_VERBOSE)
            message(STATUS "initialize_remote_FS : command : ${remote_command}")
        endif()

        execute_process(COMMAND
            bash "-c" "${remote_command}"
            COMMAND_ERROR_IS_FATAL ANY
            RESULT_VARIABLE     result
            ERROR_VARIABLE      error
        )
        if (NOT (result STREQUAL "0"))
            message(FATAL_ERROR " initialize_remote_FS :\n"
                " - error : ${error}"
                " - return : ${result}"
            )
        endif()
    endforeach()
endfunction()

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
            set(send_dir_command "${command_prefix} scp -r ${directory} ${SEND_TO_REMOTE_MACHINES_SSH_USER}@${destination}:${SEND_TO_REMOTE_MACHINES_PATH}")
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

function(remote_root_move_file)

    set(options VERBOSE)
    set(oneValueArgs DESTINATION_IP DESTINATION_PATH SOURCE_PATH SSHPASS_USER SSH_USER ROOT_PASSWORD)
    set(multiValueArgs)
    cmake_parse_arguments(REMOTE_ROOT__MOVE_FILE
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (NOT DEFINED REMOTE_ROOT__MOVE_FILE_DESTINATION_IP)
        message(FATAL_ERROR  "remote_root_move_file : missing mandatory argument `DESTINATION_IP`")
    endif()
    if (NOT DEFINED REMOTE_ROOT__MOVE_FILE_DESTINATION_PATH)
        message(FATAL_ERROR  "remote_root_move_file : missing mandatory argument `DESTINATION_PATH`")
    endif()
    if (NOT DEFINED REMOTE_ROOT__MOVE_FILE_SOURCE_PATH)
        message(FATAL_ERROR  "remote_root_move_file : missing mandatory argument `SOURCE_PATH`")
    endif()
    if (NOT DEFINED REMOTE_ROOT__MOVE_FILE_SSH_USER)
        message(FATAL_ERROR "remote_root_move_file : missing or invalid mandatory argument `SSH_USER`")
    endif()
    if (NOT DEFINED REMOTE_ROOT__MOVE_FILE_ROOT_PASSWORD)
        message(FATAL_ERROR "remote_root_move_file : missing or invalid mandatory argument `ROOT_PASSWORD`")
    endif()

    if (DEFINED REMOTE_ROOT__MOVE_FILE_SSHPASS_USER)
        set(command_prefix "sshpass -p '${REMOTE_ROOT__MOVE_FILE_SSHPASS_USER}'")
    else()
        set(command_prefix "")
    endif()

    set(move_file_command
        "${command_prefix} \
        ssh ${REMOTE_ROOT__MOVE_FILE_SSH_USER}@${REMOTE_ROOT__MOVE_FILE_DESTINATION_IP} \
        \"echo '${REMOTE_ROOT__MOVE_FILE_ROOT_PASSWORD}' | sudo -S rwdo install -m 644 ${REMOTE_ROOT__MOVE_FILE_SOURCE_PATH} ${REMOTE_ROOT__MOVE_FILE_DESTINATION_PATH}\""
    )
    if (DEFINED REMOTE_ROOT__MOVE_FILE_VERBOSE)
        message(STATUS "remote_root_move_file :\n"
            " - command : ${move_file_command}")
    endif()
    execute_process(COMMAND
        bash "-c" "${move_file_command}"
        COMMAND_ERROR_IS_FATAL ANY
        RESULT_VARIABLE     result
        ERROR_VARIABLE      error
        #ECHO_OUTPUT_VARIABLE
        #ECHO_ERROR_VARIABLE
    )
    if (NOT (result STREQUAL "0"))
        message(FATAL_ERROR " remote_root_move_file :\n"
            " - error : ${error}"
            " - return : ${result}"
        )
    endif()

endfunction()