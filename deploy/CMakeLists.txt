project(b4mesh_deploy)
cmake_minimum_required(VERSION 3.12)

message(STATUS "b4mesh::deploy ...")

if (NOT EXISTS /etc/nginx/qolyester.d/)
    message(STATUS "Creating destination directory /etc/nginx/qolyester.d/")
    file(MAKE_DIRECTORY "/etc/nginx/qolyester.d/")
endif()

## /add_transactions
install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/nginx/add_transaction.conf
        DESTINATION /etc/nginx/qolyester.d
)

## Configuration files generation
# forward variables to child process :
if (NOT DEFINED live_visualizer_install_dir)
    message(ERROR "b4mesh/live_visualiser did not export live_visualizer_install_dir variable")
endif()

install(CODE "set(live_visualizer_install_dir \"${live_visualizer_install_dir}\")")
set(live_visualizer_filestream_path "/tmp/blockgraph" CACHE PATH "live_visualizer : filestream path")
install(CODE "set(live_visualizer_filestream_path \"${live_visualizer_filestream_path}\")")
set(live_visualizer_refresh_rate 3000 CACHE STRING "live_visualizer : refresh rate")
install(CODE "set(live_visualizer_refresh_rate \"${live_visualizer_refresh_rate}\")")
# invoke deployement script :
#   http live_visualizer configuration
#   - /blockgraph_live_visualizer/view
#   - /blockgraph_live_visualizer/get_nodes
install(SCRIPT ./deploy.cmake)