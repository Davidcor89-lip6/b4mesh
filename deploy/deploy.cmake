
# http live_visualizer configuration
# - /blockgraph_live_visualizer/view
# - /blockgraph_live_visualizer/get_nodes

if (NOT DEFINED live_visualizer_install_dir)
    message(ERROR "live_visualizer_install_dir is not defined")
endif()
if (NOT DEFINED live_visualizer_filestream_path)
    message(ERROR "live_visualizer_filestream_path is not defined")
endif()
if (NOT DEFINED live_visualizer_refresh_rate)
    message(ERROR "live_visualizer_refresh_rate is not defined")
endif()

# see https://github.com/Davidcor89-lip6/b4mesh/blob/f84d289ab3cfeddc0a9ae3d50ce554bc3f3c2c59/include/configs.hpp#L35
#   define LIVEBLOCK_FILE "/tmp/blockgraph"

message(STATUS "live_visualizer_filestream_path set to : ${live_visualizer_filestream_path}")
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
