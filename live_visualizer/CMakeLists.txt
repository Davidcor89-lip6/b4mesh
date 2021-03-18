project(b4mesh_live_visualizer)
cmake_minimum_required(VERSION 3.12)

message(STATUS "b4mesh::live_visualizer ...")
message(STATUS "Information : this will install everything but the configuration (see b4mesh/deploy.cmake)")

set(live_visualizer_install_dir ${CMAKE_INSTALL_PREFIX}/live_visualizer CACHE PATH "path to live_visualizer install dir")
message(STATUS "Installation destination set to : ${live_visualizer_install_dir}")
if (NOT EXISTS ${live_visualizer_install_dir})
    file(MAKE_DIRECTORY "${live_visualizer_install_dir}")
endif()

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/live_visualizer.html
        ${CMAKE_CURRENT_SOURCE_DIR}/style.css
        ${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico
    DESTINATION ${live_visualizer_install_dir}
)
install(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    DESTINATION ${live_visualizer_install_dir}
)
