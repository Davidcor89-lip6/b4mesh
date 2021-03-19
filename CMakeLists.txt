project(b4mesh)
cmake_minimum_required(VERSION 3.12)

# Warning : This is not b4mesh's CMake top-level script
#           This project currently does not support CMake, please see `cmake-integration` branche

### Deployement

set(remote_machines_IP "NOT_DEFINED" CACHE PATH "deployement : list of ;-separated remotes IP")
if (remote_machines_IP MATCHES "NOT_DEFINED")
    message(ERROR "`remote_machines_IP` is a mandatory parameter")
endif()
message(STATUS "Remote machines IPs set to :")
foreach(IP IN LISTS remote_machines_IP)
    message(STATUS "- ${IP}")
endforeach()

include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)

## nginx
# - /add_transaction
install(CODE "MESSAGE(\"Deploying b4mesh (nginx endpoints)...\")")
install(CODE "include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)")
install(CODE "send_to_remote_machines(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    /etc/nginx/qolyester.d/
                FILES
                    ${CMAKE_SOURCE_DIR}/deploy/nginx/add_transaction.conf
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)

add_subdirectory(live_visualizer)

