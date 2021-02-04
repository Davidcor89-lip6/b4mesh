# **b4mesh**

Développment du projet **b4mesh** sur le materiel de **GreenCom**.

## 🏗️ How to build this project ?

The following project requires a two-step actions.

### Requirements

| Package name | Minimum release |
| ------------ | --------------- |
| CMake        | **3.14.2** *(>= 3.18.3975358 is better)* |
| GCC          | **9.3.0** *(7.5 might work)* |

### 🛠️ Toolchain generation

This cache generation, by default, only allow **`greensoftsdk`** target.  
Thus, `build all` is allowed, which is convinient.

```bash
# Generate a Project Buildsystem
#   cmake [<options>] <path-to-source>
#   cmake [<options>] <path-to-existing-build>
#   cmake [<options>] -S <path-to-source> -B <path-to-build>

mkdir /path/to/build
cd /path/to/build
cmake /path/to/sources # Generate cache
```

Then, build **`greensoftsdk`** target

```bash
# CMake provides a command-line signature to build an already-generated project binary tree:
#    cmake --build <dir> [<options>] [-- <build-tool-options>]
cmake --build "path/to/build"
```

### 🛠️ Project build

Generate another cache for the project, or override the same one.  
To enable project build instead of toolchain generation, you need to specify a valid path to a toolchain which is part of a Green SDK installation directory.

```cmake
cmake -DCMAKE_TOOLCHAIN_FILE:FILEPATH="/path/to/GreenSDK_toolchain_install/toolchainfile.cmake"
```

Then build the project
```cmake
cmake --build "/path/to/project/build"
```

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

# Project build :

/root/.vs/cmake/bin/cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE:STRING="Debug" -DCMAKE_INSTALL_PREFIX:PATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug-WithGreenSDK" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/install/Linux-GCC-Debug_target_greensoftsdk/toolchain/greensoftsdk/share/buildroot/toolchainfile.cmake"  "/root/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/src/CMakeLists.txt";

cd $HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK;/root/.vs/cmake/bin/cmake --build "$HOME/.vs/b4mesh/04888722-7ab6-435b-947b-dca7becdfea8/out/build/Linux-GCC-Debug-WithGreenSDK"  ;
```

*Nb : Makefile usage is `deprecated`*.
