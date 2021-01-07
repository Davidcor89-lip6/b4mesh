# b4mesh
Développment du projet b4mesh sur le materiel de GreenCom.

## buildroot configuration / toolchain compilation

extract archives/greensoft-sdk-2020-10-06-67b51.tar.xz somewhere else

copy libdbus-cpp-6d390205.tar.gz in the folder "dl" int the buildroot folder

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

$make toolchain

WAIT !!!

## b4mesh compilation

In the makefile, you need to change the link the greencommunication toolchain.

$ make all
