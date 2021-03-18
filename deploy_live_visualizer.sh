#!/bin/bash

List_IP=("10.181.178.217" "10.181.172.130" "10.154.134.26" "10.154.134.170" "10.181.178.210")

for addr in "${List_IP[@]}"
do
    echo $addr
    sshpass -p 'b4meshroot' ssh default@$addr "mkdir -p /var/persistent-data/b4mesh/tmp/"
    sshpass -p 'b4meshroot' scp CMakeLists.txt default@$addr:/var/persistent-data/b4mesh/tmp/
    sshpass -p 'b4meshroot' scp -r live_visualizer/ default@$addr:/var/persistent-data/b4mesh/tmp/
    sshpass -p 'b4meshroot' scp -r deploy/ default@$addr:/var/persistent-data/b4mesh/tmp/
done
