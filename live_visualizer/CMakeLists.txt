project(b4mesh_live_visualizer)
cmake_minimum_required(VERSION 3.12)

include(${CMAKE_SOURCE_DIR}/cmake_modules/get_nodes_filestream_path.cmake)
include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)

message(STATUS "b4mesh::live_visualizer ...")

### live_visualizer/configuration.js
# argument : live_visualizer_refresh_rate
set(live_visualizer_refresh_rate 3000 CACHE STRING "live_visualizer : refresh rate")
message(STATUS " - b4mesh::live_visualizer : refresh rate set to : ${live_visualizer_refresh_rate}")

# argument : live_visualizer_install_dir
set(live_visualizer_install_dir "/var/persistent-data/b4mesh/live_visualizer" CACHE STRING "live_visualizer : install directory (remote)")
message(STATUS " - b4mesh::live_visualizer : (remote) install dir set to : ${live_visualizer_install_dir}")

# argument : nodes_filestream_path
get_nodes_filestream_path() # generate `nodes_filestream_path`
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

# sshpass -p 'b4meshroot' scp /home/david/Documents/b4mesh/nginx/add_transaction.conf default@10.181.178.217:/tmp
# sshpass -p 'b4meshroot' ssh default@10.181.178.217
#   "echo b4meshroot | su - root rwdo cp /tmp/add_transaction.conf /etc/nginx/qolyester.d/"

# sshpass -p 'b4meshroot' ssh default@10.181.178.217
# echo 'b4meshroot' | su - root rwdo cp /tmp/add_transaction.conf /etc/nginx/qolyester.d/

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
