message(STATUS "deploy b4mesh ...")

# /add_transactions
install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/add_transaction.conf
    DESTINATION /etc/nginx/qolyester.d
)

# http live_visualizer configuration
# - /blockgraph_live_visualizer/view
# - /blockgraph_live_visualizer/get_nodes

set(live_visualizer_install_dir "NOT_DEFINED" CACHE PATH "path to live_visualizer install dir")
if (live_visualizer_install_dir STREQUAL "NOT_DEFINED")
    message(ERROR "[deploy.cmake] live_visualizer_install_dir is not defined")
endif()

# see https://github.com/Davidcor89-lip6/b4mesh/blob/f84d289ab3cfeddc0a9ae3d50ce554bc3f3c2c59/include/configs.hpp#L35
#   define LIVEBLOCK_FILE "/tmp/blockgraph"
set(live_visualizer_filestream_path "/tmp/blockgraph" CACHE PATH "path to live_visualizer filestream path")
file (
    WRITE /etc/nginx/qolyester.d/http_live_visualizer.conf
    CONTENT
        "location /blockgraph_live_visualizer/view {\n
            default_type \"text/html\";\n
            alias /path/to/live_visualizer.html;\n
        }\n
        \n
        location /blockgraph_live_visualizer/get_nodes {\n
            alias ${live_visualizer_filestream_path}; #b4mesh node endpoint\n
        }"
)
set(live_visualizer_refresh_rate 3000)
file (
    WRITE ${live_visualizer_install_dir}/configuration.js
    CONTENT
        "const configuration = {\n
            blockgraph_as_dot_API_accesspoint: \"${live_visualizer_filestream_path}\",\n
            refresh_rate_ms: ${live_visualizer_refresh_rate}\n
        }"
)