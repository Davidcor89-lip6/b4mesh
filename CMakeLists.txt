project(b4mesh)
cmake_minimum_required(VERSION 3.12)

# Warning : This is not b4mesh's real CMake top-level script
#           This project currently does not support CMake, please see `cmake-integration` branche

### Deployement

set(remote_machines_IP "NOT_DEFINED" CACHE PATH "deployement : list of ;-separated remotes IP")
if (remote_machines_IP MATCHES "NOT_DEFINED")
    message(FATAL_ERROR "`remote_machines_IP` is a mandatory parameter")
endif()
message(STATUS "${PROJECT_NAME} : Remote machines IPs set to :")
foreach(IP IN LISTS remote_machines_IP)
    message(STATUS "- ${IP}")
endforeach()

set(remote_install_dir "NOT_DEFINED" CACHE PATH "deployement : remote install dir")
if (remote_install_dir MATCHES "NOT_DEFINED")
    message(FATAL_ERROR "`remote_install_dir` is a mandatory parameter")
endif()
include(${CMAKE_SOURCE_DIR}/cmake_modules/utility.cmake)
unquote_string(${remote_install_dir} remote_install_dir)

message(STATUS "${PROJECT_NAME} : remote_install_dir set to : ${remote_install_dir}")

include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)

## remote FS initialization
install(CODE "MESSAGE(\"Initializing remote directories...\")")
install(CODE "initialize_remote_FS(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    ${remote_install_dir}
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)

## nginx
# - /add_transaction
install(CODE "MESSAGE(\"Deploying b4mesh (nginx endpoints)...\")")
install(CODE "include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)")
install(CODE "send_to_remote_machines(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    /tmp
                FILES
                    ${CMAKE_SOURCE_DIR}/nginx/add_transaction.conf
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)
foreach(IP IN LISTS remote_machines_IP)
    install(CODE "remote_root_move_file(
        DESTINATION_IP      ${IP} 
        SOURCE_PATH         /tmp/add_transaction.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )")
endforeach()

add_subdirectory(live_visualizer)

