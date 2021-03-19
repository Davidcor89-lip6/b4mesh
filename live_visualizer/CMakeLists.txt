project(b4mesh_live_visualizer)
cmake_minimum_required(VERSION 3.12)

# include(${CMAKE_SOURCE_DIR}/cmake_modules/get_nodes_filestream_path.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake_modules/send_to_remote_machines.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/get_nodes_filestream_path.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/remote_deploy.cmake)

message(STATUS "b4mesh::live_visualizer ...")

#add_custom_target(b4mesh_live_visualizer)

# set(remote_machines_IP "NOT_DEFINED" CACHE PATH "deployement : list of ;-separated remotes IP")
# if (remote_machines_IP MATCHES "NOT_DEFINED")
#     message(ERROR "`remote_machines_IP` is a mandatory parameter")
# endif()

### live_visualizer/configuration.js
# argument : live_visualizer_refresh_rate
set(live_visualizer_refresh_rate 3000 CACHE STRING "live_visualizer : refresh rate")
message(STATUS "live_visualizer_refresh_rate set to : ${live_visualizer_refresh_rate}")

# argument : nodes_filestream_path
get_nodes_filestream_path() # generate `nodes_filestream_path`
message(STATUS "nodes filestream detected on : [${nodes_filestream_path}]")
set(live_visualizer_filestream_path ${nodes_filestream_path})

# or configure() ?
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js)
file (
    WRITE ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js
    CONTENT
        "const configuration = {\n
            blockgraph_as_dot_API_accesspoint: \"${nodes_filestream_path}\",\n
            refresh_rate_ms: ${live_visualizer_refresh_rate}\n
        }"
)

install(CODE "MESSAGE(\"Deploying b4mesh::live_visualizer...\")")
install(CODE "include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/remote_deploy.cmake)")
install(CODE "send_to_remote_machines(
                DESTINATIONS
                    \"10.181.178.217\"
                    \"10.181.172.130\"
                    \"10.154.134.26\"
                    \"10.154.134.170\"
                    \"10.181.178.210\"
                PATH            /path/to/destination
                FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/live_visualizer.html
                    ${CMAKE_CURRENT_SOURCE_DIR}/style.css
                    ${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/configuration.js
                DIRECTORIES
                    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)