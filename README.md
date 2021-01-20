# b4mesh
Développment du projet b4mesh sur le materiel de GreenCom.

## configure all yours routers

use the green communication tutorial
(GreenPi / b4meshnetwork)

## buildroot configuration / toolchain compilation

extract archives/greensoft-sdk-2020-10-06-67b51.tar.xz somewhere else

copy libdbus-cpp-6d390205.tar.gz in the folder "dl" into the buildroot folder

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

Create the folder "build" and "build_local", if they don't exist.

$ make all

connect on the green communication 

$ make sendall

connect with ssh on each router (default@IP) (b4meshroot)
goes into /var/persistent-data/b4mesh
$ sudo ./b4mesh

## results

Create the folder "Results"

$ make recoverResultsAll

To visualize on result graph

$ cd Results
$ ./txt2Png blockgraph-X.txt res-X.png
($ eog res-X.png)
 
