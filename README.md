# **b4mesh**

Développment du projet **b4mesh** sur le materiel de **Green IT**.

## configure all yours routers

use the green communication tutorial
(GreenPi / b4meshnetwork)

> **todo** : link to the tutorial here

## 🏗️ How to build this project ?

The following project requires a ***two-step action***.

### Requirements

| Package name | Minimum release |
| ------------ | --------------- |
| `CMake`        | **3.14.2** *(>= 3.18.3975358 is better)* |
| `GCC`          | **9.3.0** *(7.5 might work)* |
| One of:<br>- `gmake`<br>- `nmake`<br>- `make` | ? |
| One of:<br>- `bash`<br>- `sh`      | ? |

### Dependencies management

Self-generated build diagram, using `b4mesh_generates_cmake_dependencies_diagram` target.

![Self-generated build diagram](./doc/build/dependencies_diagram.png?raw=true)

### 🛠️ Toolchain generation

This cache generation, by default, only allow **`greensoftsdk`** target.  
Thus, `build all` is allowed, which is convenient.

## b4mesh compilation

> ℹ️ *`Makefile` usage is **deprecated**, use `modern-CMake` instead*

### Using `modern-CMake`

`b4mesh` build use standard CMake cache/build process,  
with a twist that is the use of an external toolchain which is generated prior to the main build.

Thus, building the project only requires to generates CMake's cache and build **twice**.  

- The first time, the toolchain is not generated yet, thus won't be detected.  
  It will be selected as sole target, and generated then.
- The second time, the toolchain is detected. Other targets are built.

```bash
mkdir /path/to/build
cd /path/to/build
cmake /path/to/sources        # generates CMake cache
cmake --target greensoftsdk --build /path/to/build  # generates the toolchain
cmake /path/to/sources        # refresh CMake cache
cmake --build /path/to/build  # Generates other targets
```

Alternatively, you can select your target using **CMake** `--target` option explicitely.

#### Toolchain generation

```cmake
mkdir /path/to/build
cd /path/to/build
cmake /path/to/sources
cmake --target greensoftsdk /path/to/build
```

Example output, using an external `Docker` container *(Ubuntu 18.04 LTS)* as build target,  
and `Microsoft Visual Studio 2019` remote Linux compilation for CMake project default tool.

To reproduce :

- Open `Microsoft Visual Studio`
- File > Open > `CMake directory` or `directory`
- Press F7 or Project > generate cache for b4mesh
- Select `Linux-GCC-Debug_target_greensoftsdk` CMake Settings or similar
- Press F7 or Build > Build all

Possible output :

```log
Début de la copie des fichiers sur la machine distante.
Fin de la copie des fichiers (durée calendaire 00h:00m:00s:524ms).
cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk"  ;

[1/9] [greensoftsdk] add_boost_to_config step ...
-- [FileAppend.cmake] Appenning [BR2_PACKAGE_BOOST=y] to [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config]
-- [FileAppend.cmake] Appenning [BR2_PACKAGE_BOOST_SYSTEM=y] to [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config]
-- [FileAppend.cmake] Appenning [BR2_PACKAGE_LIBDBUS_CPP=y] to [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config]
-- [FileAppend.cmake] Appenning [BR2_TOOLCHAIN_BUILDROOT_WCHAR=y] to [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config]
-- [FileAppend.cmake] Appenning [BR2_PACKAGE_BOOST_RANDOM=y] to [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config]
[2/7] cd /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk && /root/.vs/cmake/bin/cmake --graphviz=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.dot . && dot -Tpng /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.dot -o /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.png
-- [b4mesh] Loading modules [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/cmake/modules] ...
-- [b4mesh] : toolchain is []
-- [b4mesh] : CMAKE_BUILD_TYPE is [Debug]
-- [toolchain/lazy_detection] : Lazy Green SDK detection ... USING_GREEN_SDK is [OFF]
-- [FindFile] Scanning [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives] for [greensoft-sdk-*.tar.xz] ...
-- [FindFile] Found    [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives/greensoft-sdk-2020-10-06-67b51.tar.xz]
-- [greensoftsdk] ...
-- [FindFile] Scanning [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives] for [libdbus-cpp-*.tar.gz] ...
-- [FindFile] Found    [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives/libdbus-cpp-6d390205.tar.gz]
-- Configuring done
-- Generating done
Generate graphviz: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.dot
-- Build files have been written to: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk
[3/7] Performing build step for 'greensoftsdk'
[greensoftsdk] Building SDK in /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk...
/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config:3503:warning: override: reassigning to symbol BR2_PACKAGE_BOOST
/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config:3504:warning: override: reassigning to symbol BR2_PACKAGE_BOOST_SYSTEM
/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config:3505:warning: override: reassigning to symbol BR2_PACKAGE_LIBDBUS_CPP
/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config:3506:warning: override: reassigning to symbol BR2_TOOLCHAIN_BUILDROOT_WCHAR
/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config:3507:warning: override: reassigning to symbol BR2_PACKAGE_BOOST_RANDOM
#
# configuration written to /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/.config
#
>>>   Finalizing host directory
>>>   Finalizing target directory
>>>   Sanitizing RPATH in target tree
>>>   Generating root filesystems common tables
>>>   Generating filesystem image rootfs.tar
rootdir=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/output/build/buildroot-fs/tar/target
table='/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/output/build/buildroot-fs/full_devices_table.txt'
>>>   Rendering the SDK relocatable
>>>   Generating SDK tarball
[greensoftsdk] Built.
[4/7] Performing install step for 'greensoftsdk'
[greensoftsdk] Installing ...
[5/7] No test step for 'greensoftsdk'
[6/7] [greensoftsdk] custom_install step ...
[greensoftsdk] Extracting [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/output/images/arm-buildroot-linux-uclibcgnueabi_sdk-buildroot.tar.gz]
[greensoftsdk] Running [relocate-sdk.sh] script ...
Relocating the buildroot SDK from /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk-prefix/src/greensoftsdk/output/host to /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk ...
[greensoftsdk] Installed in /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain
[7/7] Completed 'greensoftsdk'
Réussite de l'opération Tout générer.
```

#### 🛠️ Project build

Generate another cache for the project, or override the same one.  
To enable project build instead of toolchain generation, you need to specify a valid path to a toolchain which is part of a Green SDK installation directory.

```cmake
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH="/path/to/GreenSDK_toolchain_install/toolchainfile.cmake"
```

Then build the project
```cmake
cmake --build "/path/to/project/build"
```

If you are using Microsoft Visual Studio :

- Select `Linux-GCC-Debug-WithGreenSDK`
- Press F7 or Build > Build all
- *Wait quite a long time ...*

Possible output :

```log
cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK"  ;

[1/14] cd /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources && /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources/consensus.xml --proxy=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output/includes/b4mesh/core_engine/generated/consensus.hpp && /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources/state.xml --proxy=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output/includes/b4mesh/core_engine/generated/state.hpp
# etc ...
```

> See the [#Notes](#notes) section below for generated commands details.

### [***deprecated***] Using `Unix-Makefile`

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

#### results

Create the folder "Results"

$ make recoverResultsAll

To visualize on result graph

$ cd Results
$ ./txt2Png blockgraph-X.txt res-X.png
($ eog res-X.png)

### Notes

Feel free to add extra parameters to your `cmake` commands lines.

In particular :

- `-G "Ninja"`, `-G "Unix Makefiles"`, or any legal value for `-G` option that suit your needs.
- `-DCMAKE_BUILD_TYPE:STRING="Debug"`, to set build type.  
  ➡️ Default is `RelWithDebInfo`.

> For instance, here is how **`Microsoft Visual Studio (MsVS)`** invoke its packaged `cmake binary` when building cmake-based project with remote build on Linux target.  
Everything here use default parameters.

```bash
# toolchain generation :

/root/.vs/cmake/bin/cmake -G "Ninja"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk" -DCMAKE_CXX_COMPILER:FILEPATH="/usr/bin/g++-9" -DCMAKE_C_COMPILER:FILEPATH="/usr/bin/gcc-9" --target greensoftsdk "/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/CMakeLists.txt";

cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk"  ;

# ...
# Project build :

/root/.vs/cmake/bin/cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug-WithGreenSDK" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/share/buildroot/toolchainfile.cmake"  "/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/CMakeLists.txt";

cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK"  ;
# ...
```

*Nb : Makefile usage is `deprecated`*.


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

**NB** : Please not that `live_visualizer` target can be either standalone, or part of b4mesh top-level CMake script.

| Argument | type |  Mandatory ? | Default value |
| -------- | ---- | ------------ | ------------- |
| remote_machines_IP           | STRING<BR>*(`\;-separated` LIST)* | Yes | None |
| remote_install_dir           | PATH | No | None |
| live_visualizer_install_dir  | PATH | Yes if standalone | Only if not standalone :<br>`${remote_install_dir}`/live_visualizer |
| live_visualizer_refresh_rate | INTEGER<br>*(in milliseconds)* | No | 3000 |

#### Standalone usage

Even if `b4mesh::live_visualizer` depends on several elements from b4mesh top-level projects,  
it might be use in standalone.

Theses dependencies are :

- b4mesh/cmake_modules
- b4mesh/include/configs.hpp

```bash
# Assuming PWD is b4mesh sources's dir
mkdir build && cd build;
cmake   --target=b4mesh_live_visualizer \ # force standalone
        -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
        -Dlive_visualizer_install_dir:PATH=\"/var/persistent-data/b4mesh/live_visualizer\"                      \
        ..
cmake --install .
```

Which is equivalent to :

```bash
# Assuming PWD is b4mesh sources's dir
mkdir build && cd build;
cmake   \
        -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
        -Dlive_visualizer_install_dir:PATH=\"/var/persistent-data/b4mesh/live_visualizer\"                      \
        ../live_visualizer # explictly run live_visualizer/CMakeLists.txt, not the top-level script
cmake --install .
```

Alternatively, as mentioned in the arguments table hereabove,  
the user can substitute `live_visualizer_install_dir` parameter by `remote_install_dir`,  
which will result in `live_visualizer_install_dir` set to `${remote_install_dir}/live_visualizer`

Thus, previous command samples are equivalent to :

```bash
# Assuming PWD is b4mesh sources's dir
mkdir build && cd build;
cmake   \
        -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
        -Dremote_install_dir:PATH=\"/var/persistent-data/b4mesh\" \ # remote_install_dir here, not explicit live_visualizer_install_dir
        ../live_visualizer # explictly run live_visualizer/CMakeLists.txt, not the top-level script
cmake --install .
```

#### As part of top-level b4mesh CMake script

```bash
# Assuming PWD is b4mesh sources's dir
mkdir build && cd build;
cmake   \
        -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
        -Dremote_install_dir:PATH=\"/var/persistent-data/b4mesh\"
        ..
cmake --install .
```

Note that because we deploy remotely, standard CMake argument `CMAKE_INSTALL_PREFIX` has no real meaning here.  
We substitute it with `remote_install_dir` to set the remote installation path.

**NB** : *Note the escaping character sequence using `\"`, which prevent CMake from resolving localy the path.*

In the snippet above deployement results in the following files :

```log
-- b4mesh : Remote machines IPs set to :
-- - 10.181.178.217
-- - 10.181.172.130
-- - 10.154.134.26
-- - 10.154.134.170
-- - 10.181.178.210
-- b4mesh : remote_install_dir set to : "/var/persistent-data/b4mesh"
-- b4mesh::live_visualizer ...
--  - b4mesh::live_visualizer : refresh rate set to : 3000
--  - b4mesh::live_visualizer : nodes filestream detected on : [/tmp/blockgraph]
--  - b4mesh::live_visualizer : (remote) install dir set to : /var/persistent-data/b4mesh/live_visualizer
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Dev/b4mesh_main/build
```
