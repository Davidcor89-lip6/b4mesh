#!/bin/bash

script_path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
local_repo_path=$script_path/..

echo -e "B4mesh : Deploying [$local_repo_path] ..."

get_node_filestream() {
    files="$local_repo_path/include/*"
    regex=".*LIVEBLOCK_FILE\ *\=?\ *.*\"(.+)\"" # match compile-time constant (define or static constexpr synthaxes)
    for f in $files
    do
        if [[ $f =~ $regex ]]
        then
            match="${BASH_REMATCH[1]}"
            echo -e "Matched : [$match]\n"
            return
        else
            echo "$f doesn't match" >&2 # this could get noisy if there are a lot of non-matching files
        fi
    done
}
node_filestream_path=$(get_node_filestream)

# List_IP=("10.181.178.217" "10.181.172.130" "10.154.134.26" "10.154.134.170" "10.181.178.210")

# for addr in "${List_IP[@]}"
# do
#     echo $addr

#     sshpass -p 'b4meshroot' ssh default@$addr "mkdir -p /var/persistent-data/b4mesh/tmp/"
    
#     # live_visualizer sources ...
#     sshpass -p 'b4meshroot' scp     live_visualizer/live_visualizer.html    default@$addr:/var/persistent-data/b4mesh/live_visualizer/
#     sshpass -p 'b4meshroot' scp     live_visualizer/style.css               default@$addr:/var/persistent-data/b4mesh/live_visualizer/
#     sshpass -p 'b4meshroot' scp     live_visualizer/favicon.ico             default@$addr:/var/persistent-data/b4mesh/live_visualizer/
#     sshpass -p 'b4meshroot' scp -r  live_visualizer/scripts/                default@$addr:/var/persistent-data/b4mesh/live_visualizer/

#     # live_visualizer configurations + nginx configurations ...
#     sshpass -p 'b4meshroot' ssh default@$addr: "mkdir -p /etc/nginx/qolyester.d/" # may fail, does not matter
#     sshpass -p 'b4meshroot' scp deploy/nginx/add_transaction.conf /etc/nginx/qolyester.d/

#     sshpass -p 'b4meshroot' ssh default@$addr "echo -e \"location /blockgraph_live_visualizer/view {\n \
#         default_type \"text/html\";\n                       \
#         alias /var/persistent-data/b4mesh/live_visualizer/live_visualizer.html;\n       \
#     }\n                                                     \
#     location /blockgraph_live_visualizer/get_nodes {\n      \
#         alias /tmp/blockgraph;\n                            \
#     }\" > /etc/nginx/qolyester.d/http_live_visualizer.conf"

#     sshpass -p 'b4meshroot' ssh default@$addr "echo -e \"const configuration = {\n
#             blockgraph_as_dot_API_accesspoint: \"/tmp/blockgraph\",\n
#             refresh_rate_ms: 3000\n
#         }\" > /var/persistent-data/b4mesh/live_visualizer/configuration.js"
# done
