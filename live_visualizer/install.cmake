message(STATUS "install b4mesh::live_visualizer ...")
message(STATUS "Information : this will install everything but the configuration (see b4mesh/deploy.cmake)")

set(live_visualizer_install_dir "NOT_DEFINED" CACHE PATH "path to live_visualizer install dir")
if (live_visualizer_install_dir STREQUAL "NOT_DEFINED")
    message(ERROR "[live_visualizer/install.cmake] live_visualizer_install_dir is not defined")
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