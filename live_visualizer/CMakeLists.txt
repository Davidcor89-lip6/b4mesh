project(b4mesh_live_visualizer)
cmake_minimum_required(VERSION 3.12)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/get_nodes_filestream_path.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/remote_deploy.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/utility.cmake)

message(STATUS "b4mesh::live_visualizer ...")

# argument : remote_machines_IP (can be set by top-level script)
if (NOT DEFINED remote_machines_IP)
    set(remote_machines_IP "NOT_DEFINED" CACHE PATH "deployement : list of ;-separated remotes IP")
    if (remote_machines_IP MATCHES "NOT_DEFINED")
        message(FATAL_ERROR "`remote_machines_IP` is a mandatory parameter")
    endif()
endif()

### live_visualizer/configuration.js
# argument : live_visualizer_refresh_rate
set(live_visualizer_refresh_rate 3000 CACHE STRING "live_visualizer : refresh rate")
message(STATUS " - b4mesh::live_visualizer : refresh rate set to : ${live_visualizer_refresh_rate}")

# argument : live_visualizer_install_dir
if (NOT DEFINED remote_install_dir) # (can be set by top-level script)
    set(live_visualizer_install_dir "NOT_DEFINED" CACHE STRING "live_visualizer : install directory (remote)")
    if (live_visualizer_install_dir STREQUAL "NOT_DEFINED")
        message(FATAL_ERROR " `live_visualizer_install_dir` is a mandatory parameter"
                            " (only if `remote_install_dir` is not set by top-level script)")
    endif()
else()
    unquote_string(${remote_install_dir} remote_install_dir)
    set(live_visualizer_install_dir "${remote_install_dir}/live_visualizer" CACHE STRING "live_visualizer : install directory (remote)")
    string(REPLACE "\"" "" live_visualizer_install_dir "${live_visualizer_install_dir}") # because `unquote` append before set
endif()
unquote_string(${live_visualizer_install_dir} live_visualizer_install_dir)
message(STATUS " - b4mesh::live_visualizer : (remote) install dir set to : ${live_visualizer_install_dir}")

# argument : nodes_filestream_path
get_nodes_filestream_path(${CMAKE_CURRENT_SOURCE_DIR}/../sources) # generate `nodes_filestream_path`
message(STATUS " - b4mesh::live_visualizer : nodes filestream detected on : [${nodes_filestream_path}]")
set(live_visualizer_filestream_path ${nodes_filestream_path})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js.in
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js
)

## Nginx endpoints
# - /blockgraph_live_visualizer/view
# - /blockgraph_live_visualizer/get_nodes
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nginx/http_live_visualizer.conf.in
    ${CMAKE_CURRENT_SOURCE_DIR}/nginx/http_live_visualizer.conf
)

### Deployement at install-time
# nginx
# [Note] :  As the FS-tree /etc/ is read-only, we need to first store datas into /tmp,
#           and only then move it to /etc/nginx/qolyester.d/
install(CODE "MESSAGE(\"Deploying b4mesh::live_visualizer (nginx endpoints)...\")")
install(CODE "include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)")
install(CODE "send_to_remote_machines(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    /tmp
                FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/nginx/http_live_visualizer.conf
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)
foreach(IP IN LISTS remote_machines_IP)
    install(CODE "remote_root_move_file(
        DESTINATION_IP      ${IP} 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )")
endforeach()

# sources
install(CODE "MESSAGE(\"Deploying b4mesh::live_visualizer (sources)...\")")
install(CODE "include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)")
install(CODE "send_to_remote_machines(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    ${live_visualizer_install_dir}
                FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/index.html
                    ${CMAKE_CURRENT_SOURCE_DIR}/style.css
                    ${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js
                DIRECTORIES
                    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)

## remote FS initialization
install(CODE "MESSAGE(\"Fixing remote directories ACLs for NGinx ...\")")
install(CODE "initialize_remote_FS(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    ${remote_install_dir}
                SSHPASS_USER    b4meshroot
                SSH_USER        default
                ROOT_PASSWORD   b4meshroot
            )"
)

## remote restart nginx
foreach(IP IN LISTS remote_machines_IP)
    install(CODE "remote_restart_nginx(
        DESTINATION_IP      ${IP} 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )")
endforeach()