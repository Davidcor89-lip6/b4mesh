# **b4mesh**

Développment du projet **b4mesh** sur le materiel de **Green IT**.

## Components descriptions

| Component name | Visibility  | Description |
| -------------- | ----------- | ----------- |
| `b4mesh::binary`          | external | Node instance which runs on each router |
| `b4mesh::live_visualizer` | external | Web application which creates a live rendering of the blockgraph,<br>from the current node perspective |
| `b4mesh::httpd`           | internal | Httpd interface<br>used in `b4mesh::binary` |
| `b4mesh::core_engine`     | internal | blockgraph implementation<br>used in `b4mesh::binary` |
| `b4mesh::utils`           | internal | `b4mesh::binary` technical utilities/misc |

## Routers configuration

Use the green communication tutorial *(GreenPi/b4meshnetwork)*

> **todo** : link to the tutorial here

## 🏗️ How to build this project ?

The following project build requires a ***two-step action***.

### Requirements

| Package name | Minimum release |
| ------------ | --------------- |
| `CMake`                       | **3.14.2** *(>= 3.18.3975358 is better)* |
| `GCC`                         | **9.3.0** *(7.5 might work)* |
| `gmake`, `nmake` or `make`    | ? |
| `bash` or `sh`                | ? |

### Dependencies management

Self-generated build diagram, using `b4mesh_generates_cmake_dependencies_diagram` target.

![Self-generated build diagram](./doc/build/dependencies_diagram.png?raw=true)

### 🛠️ Toolchain generation

The first cache generation, by default, only allow a target named **`greensoftsdk`**.  
This target will generate Green's toolchain.

#### **Using `command-line` and `Unix-Makefile`**

> ℹ️ Command-line configuration and *`Makefile` usage is **deprecated**, use `modern-CMake` instead*

#### **Using `modern-CMake`**

`b4mesh` build use standard CMake cache/build process,  
with a twist that is the use of an external toolchain which must be generated prior to the main build.  
Thus, building the project only requires to generates CMake's cache and build **twice**.  

- The first time, the toolchain is not generated yet, thus won't be detected.  
  It will be selected as sole target - *`greensoftsdk`* -.
- The second time, the toolchain is detected. Other targets are generated.

> ℹ️ If your IDE supports it, you may be willing to use the provided [CMakeSettings.json](./CMakeSettings.json) file to handle your CMake cache/build command lines.  
> Alternatively, you can select your target using **CMake** `--target` option explicitely.

> ℹ️ Feel free to add extra options that best fit your needs, such as `-G` or `--parallel`/`-j`.

**NB** : By default, `greensoftsdk` target install the output toolchain into `${CMAKE_INSTALL_PREFIX}/toolchain`,  
so you might want to set the `-DCMAKE_INSTALL_PREFIX:PATH="/path/to/install/"` option when generating the toolchain.

Toolchain generation exposes the following CMake targets :

| name | description |
| ---- | ----------- |
| **greensoftsdk** | Green IT SDK/toolchain, on which relies `b4mesh` |

##### **Toolchain generation** (command line)

Here, we use `${SOURCE_DIR}/build` as cache directory for `CMake`, and `ninja` as generator.

- CMake >= `3.13.5`

  ```bash
  cd path/to/source && mkdir build;
  # cache generation
  cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE:STRING="Release" \
        -DCMAKE_INSTALL_PREFIX:PATH="/path/to/install" \
        --target greensoftsdk \
        -S . -B ./build/
  # build
  cmake --build ./build/
  ```

- CMake <= `3.12`

  ```bash
  # cache genreation
  cd /path/to/sources/
  mkdir build && cd build
  cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE:STRING="Release" \
        -DCMAKE_INSTALL_PREFIX:PATH="/path/to/install" \
        --target greensoftsdk \
        ..
  
  # build
  cmake --build .
  ```

> Note about the build time :  
> *The toolchain build takes quite a while. Expect >= 2 hours using a I5-8250U CPU and 8 Go RAM for instance*

##### **Toolchain generation** (Microsoft Visual Studio cross-plateform)

Example output, using an external `Docker` container *(Ubuntu 18.04 LTS)* as build target,  
and `Microsoft Visual Studio 2019` cross-plateform, remote Linux compilation for CMake project default tool.

To reproduce :

- Open `Microsoft Visual Studio`
- File > Open > `CMake directory` or `directory`
- Press F7 or Project > generate cache for b4mesh
- Select `Linux-GCC-Debug_target_greensoftsdk` CMake Settings or similar
- Press F7 or Build > Build all

<details><summary>Possible cache generation output</summary>
<p>
```log
1> Copie des fichiers sur la machine distante.
1> Début de la copie des fichiers sur la machine distante.
1> [rsync] rsync -t --delete --delete-excluded -v -r --exclude=.vs --exclude=.git --exclude=out -8  "." rsync://root@localhost:63708/-root-.vs-b4mesh-04888722-7ab6-435b-947b-dca7becdfea8-src
1> [rsync] sending incremental file list
1> [rsync] ./
1> [rsync] README.md
1> [rsync] deleting live_visualizer/configuration.js
1> [rsync] analysis/clang-tidy_output.txt
1> [rsync] doc/build/
1> [rsync] doc/build/dependencies_diagram.dot
1> [rsync] doc/build/dependencies_diagram.dot.Boostrandom
1> [rsync] doc/build/dependencies_diagram.dot.Boostrandom.dependers
1> [rsync] doc/build/dependencies_diagram.dot.Boostsystem
1> [rsync] doc/build/dependencies_diagram.dot.Boostsystem.dependers
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX.dependers
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX_asio_integration
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX_asio_integration.dependers
1> [rsync] doc/build/dependencies_diagram.dot.ThreadsThreads
1> [rsync] doc/build/dependencies_diagram.dot.ThreadsThreads.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_binary
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_binary.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_coreEngine
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_coreEngine.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_core_engine__generated
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_core_engine__generated.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_http
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_http.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_utils
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_utils.dependers
1> [rsync] doc/build/dependencies_diagram.dot.boost_beast
1> [rsync] doc/build/dependencies_diagram.dot.boost_beast.dependers
1> [rsync] doc/build/dependencies_diagram.dot.nlohmann_json
1> [rsync] doc/build/dependencies_diagram.dot.nlohmann_json.dependers
1> [rsync] doc/build/dependencies_diagram.png
1> [rsync] live_visualizer/
1> [rsync] live_visualizer/nginx/
1> [rsync] live_visualizer/nginx/http_live_visualizer.conf
1> [rsync] sources/utils/
1> [rsync] sources/utils/CMakeLists.txt
1> [rsync] 
1> [rsync] sent 176,941 bytes  received 6,992 bytes  73,573.20 bytes/sec
1> [rsync] total size is 10,008,738  speedup is 54.42
1> Fin de la copie des fichiers (durée calendaire 00h:00m:04s:129ms).
1> La génération de CMake a démarré pour la configuration : 'Linux-GCC-Debug_target_greensoftsdk'.
1> Exécutable cmake trouvé sur /root/.vs/cmake/bin/cmake.
1> /root/.vs/cmake/bin/cmake -G "Ninja"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk" -DCMAKE_CXX_COMPILER:FILEPATH="/usr/bin/g++-9" -DCMAKE_C_COMPILER:FILEPATH="/usr/bin/gcc-9" --target greensoftsdk "/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/CMakeLists.txt";
1> [CMake] -- [b4mesh] Loading modules [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/cmake/modules] ...
1> [CMake] -- The CXX compiler identification is GNU 9.3.0
1> [CMake] -- Detecting CXX compiler ABI info
1> [CMake] -- Detecting CXX compiler ABI info - done
1> [CMake] -- Check for working CXX compiler: /usr/bin/g++-9 - skipped
1> [CMake] -- Detecting CXX compile features
1> [CMake] -- Detecting CXX compile features - done
1> [CMake] -- [b4mesh] : option [b4mesh_USE_CLANG_TIDY] set to OFF
1> [CMake] -- [b4mesh] : program [dot] detected at /usr/bin/dot. Generating [b4mesh_generates_cmake_dependencies_diagram] target
1> [CMake] -- [b4mesh] : toolchain is []
1> [CMake] -- [b4mesh] : CMAKE_BUILD_TYPE is [Debug]
1> [CMake] -- [toolchain/lazy_detection] : Lazy Green SDK detection ... USING_GREEN_SDK is [OFF]
1> [CMake] -- [FindFile] Scanning [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives] for [greensoft-sdk-*.tar.xz] ...
1> [CMake] -- [FindFile] Found    [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives/greensoft-sdk-2021-01-25-14ae3.tar.xz]
1> [CMake] -- [greensoftsdk] ...
1> [CMake] -- [FindFile] Scanning [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives] for [libdbus-cpp-*.tar.gz] ...
1> [CMake] -- [FindFile] Found    [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/toolchain/archives/libdbus-cpp-e02d9095.tar.gz]
1> [CMake] -- Configuring done
1> [CMake] -- Generating done
1> [CMake] -- Build files have been written to: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug_target_greensoftsdk
1> Variables CMake extraites.
1> Fichiers sources et en-têtes extraits.
1> Modèle de code extrait.
1> Chemins include extraits.
1> Fin de la génération de CMake.
```
</p>
</details>

<details><summary>Possible build output : </summary>
<p>

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
</p></details>

### 🛠️ Project build

The following steps describe how to build b4mesh projects components.  
This requires that previous steps was succesfuly followed (see the [toolchain generation section](#️-toolchain-generation) above).

#### **Using `modern-CMake`**

The project's top-level CMakeLists.txt exposes the following targets :

| name | description |
| ---- | ----------- |
| **b4mesh_binary**                                 | `b4mesh::binary`<br>project's main output (executable) |
| **b4mesh_coreEngine**                             | `b4mesh::core_engine`<br>on which depends **b4mesh_binary**<br>*(see [#dependencies_management section](#dependencies-management))* |
| **b4mesh_core_engine__generate_ressources**       | Generates ressources on which depends b4mesh::core_engine|
| **b4mesh_generates_cmake_dependencies_diagram**   | Generates all build target dependencies diagrams, available in <project_dir>/doc/build/ |

##### **Project build (command lines)**

Generate another cache for the project, or override the same one.  
To enable project build instead of toolchain generation, you need to specify a valid path to a toolchain which is part of a Green SDK installation directory.

**NB** : By default, `greensoftsdk` target install the output toolchain into `${CMAKE_INSTALL_PREFIX}/toolchain`,  
so you might want to set the `-DCMAKE_INSTALL_PREFIX:PATH="/path/to/install/"` option when generating the toolchain.

```bash
mkdir build && cd build
cmake \
    -G "Ninja"                                 \   # your favorite generator
    -DCMAKE_BUILD_TYPE:STRING="RelWithDebInfo" \   # Debug, Release, RelWithDebInfo or MinSizeRel
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="/path/to/GreenSDK_toolchain_install/toolchainfile.cmake" \ # toolchain
    \ # b4mesh specific options :
    -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" \
    -Dremote_install_dir:PATH="/var/persistent-data/b4mesh"                                                 \
    -Db4mesh_USE_CLANG_TIDY:BOOL=OFF                                                                        \
    ..
```

> ℹ️ B4mesh::live_visualizer specific CMake options are detailed [here](#configuration-installation-deployement)

> ℹ️ If you plan to locally install your project outputs : 
> - do not forget to add an `-DCMAKE_INSTALL_PREFIX:PATH="/path/to/install/"` option,  
> - and invoke a `cmake --install .` command after the build as well.

Then build the project, using `cmake --build`,  
or manually invoque your generator *(`Unix-Makefile`, `Ninja`, etc.)*

```bash
cmake --build "/path/to/project/build" . # assuming '.' is your cache path
```

##### **Project build (Ms Visual Studio)**

If you are using Microsoft Visual Studio :

- Select `Linux-GCC-Debug-WithGreenSDK`
- Press F7 or Build > Build all

<details><summary>Possible cache generation output</summary>
<p>
```log
1> Copie des fichiers sur la machine distante.
1> Début de la copie des fichiers sur la machine distante.
1> [rsync] rsync -t --delete --delete-excluded -v -r --exclude=.vs --exclude=.git --exclude=out -8  "." rsync://root@localhost:63484/-root-.vs-b4mesh-04888722-7ab6-435b-947b-dca7becdfea8-src
1> [rsync] sending incremental file list
1> [rsync] ./
1> [rsync] CMakeSettings.json
1> [rsync] README.md
1> [rsync] deleting live_visualizer/configuration.js
1> [rsync] cmake_modules/
1> [rsync] doc/build/
1> [rsync] doc/build/dependencies_diagram.dot
1> [rsync] doc/build/dependencies_diagram.dot.Boostrandom
1> [rsync] doc/build/dependencies_diagram.dot.Boostrandom.dependers
1> [rsync] doc/build/dependencies_diagram.dot.Boostsystem
1> [rsync] doc/build/dependencies_diagram.dot.Boostsystem.dependers
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX
1> [rsync] doc/build/dependencies_diagram.dot.DBusCXX.dependers
1> [rsync] doc/build/dependencies_diagram.dot.INTERFACE
1> [rsync] doc/build/dependencies_diagram.dot.INTERFACE.dependers
1> [rsync] doc/build/dependencies_diagram.dot.ThreadsThreads
1> [rsync] doc/build/dependencies_diagram.dot.ThreadsThreads.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_binary
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_binary.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_coreEngine
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_coreEngine.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_core_engine__generated
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_core_engine__generated.dependers
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_utils
1> [rsync] doc/build/dependencies_diagram.dot.b4mesh_utils.dependers
1> [rsync] doc/build/dependencies_diagram.png
1> [rsync] live_visualizer/
1> [rsync] live_visualizer/nginx/
1> [rsync] live_visualizer/nginx/http_live_visualizer.conf
1> [rsync] 
1> [rsync] sent 18,099 bytes  received 1,962 bytes  5,731.71 bytes/sec
1> [rsync] total size is 9,995,493  speedup is 498.25
1> Fin de la copie des fichiers (durée calendaire 00h:00m:03s:751ms).
1> La génération de CMake a démarré pour la configuration : 'Linux-GCC-Debug-WithGreenSDK'.
1> Exécutable cmake trouvé sur /root/.vs/cmake/bin/cmake.
1> /root/.vs/cmake/bin/cmake -G "Ninja"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug-WithGreenSDK" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/share/buildroot/toolchainfile.cmake" -Dremote_machines_IP:STRING="10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210" -Dremote_install_dir:PATH="/var/persistent-data/b4mesh"  "/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/CMakeLists.txt";
1> [CMake] -- [b4mesh] Loading modules [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/cmake/modules] ...
1> [CMake] -- The CXX compiler identification is GNU 9.3.0
1> [CMake] -- Detecting CXX compiler ABI info
1> [CMake] -- Detecting CXX compiler ABI info - done
1> [CMake] -- Check for working CXX compiler: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/arm-buildroot-linux-uclibcgnueabi-g++ - skipped
1> [CMake] -- Detecting CXX compile features
1> [CMake] -- Detecting CXX compile features - done
1> [CMake] -- [b4mesh] : toolchain is [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/share/buildroot/toolchainfile.cmake]
1> [CMake] -- [b4mesh] : CMAKE_BUILD_TYPE is [Debug]
1> [CMake] -- [toolchain/lazy_detection] : Lazy Green SDK detection ... USING_GREEN_SDK is [ON]
1> [CMake] -- [toolchain/import.cmake] Importing GreenSDK ...
1> [CMake] -- [toolchain/import.cmake] custom GreenSDK variable RELOCATED_HOST_DIR=[/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk]...
1> [CMake] -- [toolchain/import.cmake] CMAKE_CXX_COMPILER=[/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/arm-buildroot-linux-uclibcgnueabi-g++]
1> [CMake] -- [libdbus-cpp] Loading dependency ...
1> [CMake] -- Found Boost: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/include (found suitable version "1.73.0", minimum required is "1.65.0") found components: system 
1> [CMake] -- [b4mesh] core_engine/ressources : detecting [dbusxx-xml2cpp] at [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp]
1> [CMake] -- [b4mesh] core_engine/ressources : output is generated in [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output]
1> [CMake] -- Found Boost: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/include (found suitable version "1.73.0", minimum required is "1.65.0") found components: random system 
1> [CMake] -- Found Boost: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/include (found suitable version "1.73.0", minimum required is "1.65.0") found components: system 
1> [CMake] -- Looking for C++ include pthread.h
1> [CMake] -- Looking for C++ include pthread.h - found
1> [CMake] -- Performing Test CMAKE_HAVE_LIBC_PTHREAD
1> [CMake] -- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
1> [CMake] -- Found Threads: TRUE  
1> [CMake] -- b4mesh : Remote machines IPs set to :
1> [CMake] -- - 10.181.178.217
1> [CMake] -- - 10.181.172.130
1> [CMake] -- - 10.154.134.26
1> [CMake] -- - 10.154.134.170
1> [CMake] -- - 10.181.178.210
1> [CMake] -- b4mesh : remote_install_dir set to : /var/persistent-data/b4mesh
1> [CMake] -- The C compiler identification is GNU 9.3.0
1> [CMake] -- Detecting C compiler ABI info
1> [CMake] -- Detecting C compiler ABI info - done
1> [CMake] -- Check for working C compiler: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/arm-buildroot-linux-uclibcgnueabi-gcc - skipped
1> [CMake] -- Detecting C compile features
1> [CMake] -- Detecting C compile features - done
1> [CMake] -- b4mesh::live_visualizer ...
1> [CMake] --  - b4mesh::live_visualizer : refresh rate set to : 3000
1> [CMake] --  - b4mesh::live_visualizer : (remote) install dir set to : /var/persistent-data/b4mesh/live_visualizer
1> [CMake] --  - b4mesh::live_visualizer : nodes filestream detected on : [/tmp/blockgraph]
1> [CMake] -- Configuring done
1> [CMake] -- Generating done
1> [CMake] -- Build files have been written to: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK
1> Variables CMake extraites.
1> Fichiers sources et en-têtes extraits.
1> Modèle de code extrait.
1> Chemins include extraits.
```

</p>
</details>

<details><summary>Possible build output</summary>
<p>

```log
cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK"  ;

[1/14] cd /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources && /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources/consensus.xml --proxy=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output/includes/b4mesh/core_engine/generated/consensus.hpp && /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/sources/core_engine/ressources/state.xml --proxy=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output/includes/b4mesh/core_engine/generated/state.hpp
generating code for interface net.qolsr.Consensus...
generating code for interface net.qolsr.Qolyester.State...
generating code for interface net.qolsr.Qolyester.Interfaces...
generating code for interface net.qolsr.Qolyester.PeerSignals...
generating code for interface net.qolsr.Qolyester.PendingInterfaces...
generating code for interface net.qolsr.Qolyester.Links...
generating code for interface net.qolsr.Qolyester.Neighbors...
generating code for interface net.qolsr.Qolyester.MIDs...
generating code for interface net.qolsr.Qolyester.HNAs...
generating code for interface net.qolsr.Qolyester.Gates...
generating code for interface net.qolsr.Qolyester.LocalTopology...
generating code for interface net.qolsr.Qolyester.GlobalTopology...
generating code for interface net.qolsr.Qolyester.ReachableNodes...
generating code for interface net.qolsr.Qolyester.ExcludedMACs...
generating code for interface net.qolsr.Qolyester.Stations...
generating code for interface net.qolsr.Qolyester.Associations...
generating code for interface net.qolsr.Qolyester.VLANs...
generating code for interface net.qolsr.Qolyester.Log...
generating code for interface net.qolsr.Qolyester.Parameters...
[2/14] Building CXX object sources/core_engine/CMakeFiles/b4mesh_coreEngine.dir/sources/consensus.cpp.o
[3/12] cd /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK && /root/.vs/cmake/bin/cmake --graphviz=/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.dot . && dot -Tpng /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.dot -o /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/doc/build/dependencies_diagram.png
-- [b4mesh] Loading modules [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/cmake/modules] ...
-- [b4mesh] : toolchain is [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/share/buildroot/toolchainfile.cmake]
-- [b4mesh] : CMAKE_BUILD_TYPE is [Debug]
-- [toolchain/lazy_detection] : Lazy Green SDK detection ... USING_GREEN_SDK is [ON]
-- [toolchain/import.cmake] Importing GreenSDK ...
-- [toolchain/import.cmake] custom GreenSDK variable RELOCATED_HOST_DIR=[/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk]...
-- [toolchain/import.cmake] CMAKE_CXX_COMPILER=[/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/arm-buildroot-linux-uclibcgnueabi-g++]
-- [libdbus-cpp] Loading dependency ...
-- [b4mesh] core_engine/ressources : detecting [dbusxx-xml2cpp] at [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/bin/dbusxx-xml2cpp]
-- [b4mesh] core_engine/ressources : output is generated in [/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK/sources/core_engine/ressources/generation_output]
-- Found Boost: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/include (found suitable version "1.73.0", minimum required is "1.65.0") found components: random system 
-- Found Boost: /root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/include (found suitable version "1.73.0", minimum required is "1.65.0") found components: system 
-- b4mesh : Remote machines IPs set to :
# etc ...
```

</p></details>

> See the [#Notes](#notes) section below for generated commands details.

#### [***deprecated***] Using `Unix-Makefile`

<details><summary>Show this deprecated section</summary>
<p>

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

##### results

Create the folder "Results"

$ make recoverResultsAll

To visualize on result graph

$ cd Results
$ ./txt2Png blockgraph-X.txt res-X.png
($ eog res-X.png)

</p>
</details>

### **Notes**

Also, if your CMake release is fresh enough, you are strongly encouraged to use the modern invocation synthax :

```bash
cmake -S . -B build # etc.
```

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



## Nginx configuration

### Automatically

See [Configuration, installation, deployement](#configuration-installation-deployement) section below

### Manually ***(deprecated)***

<details><summary>Show deprecated details</summary>
<p>

```bash
sudo rwdo -s
vi /etc/nginx/conf.d/qolyester-http.conf 
```

```bash
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

```bash
sudo nginx -s reload
```

</p>
</details>

## live visualisation

The component `b4mesh::binary` use a file, `/tmp/blockgraph` as a live stream destination for json-formated serialized nodes outputs.  
Thus, at any moment, that file contains a - *serialized* - blockgraph representation from the current node perspective, using such format :

```json
{"node":{"groupId":217,"hash":7144646,"parent":[9288542]}}
{"node":{"groupId":217,"hash":2681597,"parent":[7144646]}}
{"node":{"groupId":26,"hash":7377976,"parent":[2681597,8515655]}}
```

This format is used for live visualisation, e.g `b4mesh::live_visualizer` component

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

## Analysis

### CppCheck

Here is a command line to generate an XML report only for `b4mesh` source files :

```bash
cppcheck \
    -i ./_deps/ \
    -i ../Linux-GCC-Debug_target_greensoftsdk/ \
    -i ./CMakeFiles/ \
    --xml --enable=all --force --project=./compile_commands.json . 2> cppcheck_output.xml
```
