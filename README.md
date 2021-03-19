# b4mesh
Développment du projet b4mesh sur le materiel de GreenCom.

## configure all yours routers

use the green communication tutorial
(GreenPi-b4mesh / b4meshnetwork)

## buildroot configuration / toolchain compilation

extract archives/greensoft-sdk-2020-10-06-67b51.tar.xz somewhere else

copy libdbus-cpp-6d390205.tar.gz in the folder "dl" (that you create) into the buildroot folder

$make menuconfig

you need to activate in Toolchain
- WCHART support
- C++ support
- MMU support

in Target packages / Librairies / other / boost 
-  chrono
-  date_time
-  regex
-  serialization
-  system
-  thread

in Target packages / hardware handling
-  dbus

in External options
-  libdbus-c++ 

>$ make toolchain

WAIT !!!
Install boost1.73
in greencom/greensdk/
>$ make libdbus-cpp

Add the path of the sdk tool inside your basrc   
> export PATH="'path/to/greencomsdk/root'/output/host/bin:$PATH"   

## b4mesh compilation

In the makefile, you need to change the link the greencommunication toolchain.   
Create the folder "build" and "build_local", if they don't exist.   

>$ make all

There is two possible options to "make all" DEBUG=1 and LOCAL=1.    
DEBUG changes the compilation option from -O3 to -g3.   
LOCAL switches the compiler between arm and the local compiler

You have to change the router IP list, in two script files (sendall and recoverResult).    
>List_IP=("10.181.178.217" "10.181.172.130" "10.154.134.26" "10.154.134.170")    

and change the list in recoverDraw this the last number of the IP address.  

>List_node="130 26 217 170"

When you are connected on the green communication network, you can execute.    
You need to create on each router, a folder : */var/persistent-data/b4mesh*

>$ make sendall

Connect with ssh on each router (default@IP) (b4meshroot).  
You can transfer the script "launcher" on the router to simplify the program launch

>$ cd /var/persistent-data/b4mesh             
>$ ./launcher

Otherwise, the created program "b4mesh" can be use as is :

> sudo ./b4mesh -i myIP -p myport -r true

> Exception: Rejected send message, 1 matched rules; type="method_call", sender=":1.65888" (uid=1004 pid=5516 comm="./b4mesh -r true ") interface="org.freedesktop.DBus.Properties" member="Get" error name="(unset)" requested_reply="0" destination="net.qolsr.Consensus" (uid=1015 pid=1667 comm="/usr/sbin/qolsr-consensusd --pidfile /run/qolyeste")

means that you forget the "sudo" ;)

 + -i : indicates the IP the the Green Communication network (their is a auto detection that can failed).
 + -p : indicates the used port to communicate (5000 by default). In fact, the port number and the port number +1 is used (ex. 5000 and 5001). 
 + -r : desactives the autogeneration of transaction inside B4Mesh and open a local endpoint to register transactions from outside this program. 

 >$ curl -X PUT -d "serialiezd payload" 0.0.0.0:4242/add_transaction
 
## results

Create the folder "Results" and copy the script "recoverDraw" inside.

>$ cd Results  
>$ ./recoverDraw

You can visualize manualy using :

>$ cd Results  
>$ ./txt2Png blockgraph-X.txt res-X.png  
>$ eog res-X.png

## Nginx configuration

### Automatically

See `#Configuration, installation, deployement` section below

### Manually ***(deprecated)***
>$ sudo rwdo -s
>$ vi /etc/nginx/conf.d/qolyester-http.conf 

``` bash
server {
        listen  *:80;
        listen  [::]:80;
        include local_server_name.conf;

        ...

        include qolyester.d/*.conf;

        location /add_transaction {
                 proxy_pass http://127.0.0.1:4242/add_transaction;
        }
}
```

>$ sudo nginx -s reload

## live visualisation

In the file "/tmp/blockgraph", you can find a live feed of the blockgraph under a json format.
Each node is under the format : 

```json
{"node":{"groupId":217,"hash":7144646,"parent":[9288542]}}
{"node":{"groupId":217,"hash":2681597,"parent":[7144646]}}
{"node":{"groupId":26,"hash":7377976,"parent":[2681597,8515655]}}
```

### Configuration, installation, deployement

CMake's top-level script handle **configuration**, **install**, and **deployement** - *on remote machines* - of b4mesh's `live visualisation` component.

**NB** : Such script also handles nginx endpoints configuration, deploying into `/etc/nginx/qolyester.d` path.

> Deployement is handle by `install-time` instruction.  
> *Thus, during `cmake --install` command invocation*

| Argument | type |  Mandatory ? | Default value |
| -------- | ---- | ------------ | ------------- |
| remote_machines_IP           | STRING<BR>*(`\;-separated` LIST)* | YES | NONE |
| live_visualizer_install_dir  | STRING/PATH | No | /var/persistent-data/b4mesh/live_visualizer | 
| live_visualizer_refresh_rate | INTEGER<br>*(in milliseconds)* | NO | 3000 |


```bash
mkdir build && cd build;
cmake   --target=b4mesh_live_visualizer \
        -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
        ..
cmake --install .
```

In the snippet above, by forcing `CMAKE_INSTALL_PREFIX`, deployement results in the following files :

```log
-- Remote machines IPs set to :
-- - 10.181.178.217
-- - 10.181.172.130
-- - 10.154.134.26
-- - 10.154.134.170
-- - 10.181.178.210
-- b4mesh::live_visualizer ...
--  - b4mesh::live_visualizer : refresh rate set to : 3000
--  - b4mesh::live_visualizer : nodes filestream detected on : [/tmp/blockgraph]
--  - b4mesh::live_visualizer : (remote) install dir set to : /var/persistent-data/b4mesh/live_visualizer
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Dev/b4mesh_main/build
```
