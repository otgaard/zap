# zap

The zap engine has as few dependencies as possible.  At present, only GLFW and GLEW are
required.  GLFW is a submodule and can be checked out from Github, GLEW is included 
directly in the third_party folder.

macOS/Linux Build Instructions
------------------------------

Tested on GCC 5.4.0 && Apple LLVM 9.0.0

Checkout repository:

* git clone https://github.com/otgaard/zap

Checkout submodules:

* cd zap
* git submodule init
* git submodule update

Compile GLFW and install in zap/third_party:

* cd third_party/glfw
* mkdir build
* cd build
* cmake .. -DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_THIRD_PARTY] -DCMAKE_BUILD_TYPE=Release
* (example -DCMAKE_INSTALL_PREFIX=/home/otgaard/zap/third_party/)
* make -j 4 && make install

Compile GLEW (included in zap source code):

* cd ../../glew
* make -j 4

Build zap:

* cd ../..
* mkdir build
* cd build
* cmake .. -DCMAKE_BUILD_TYPE=[Release|Debug] [-DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_INSTALL]]
* (example -DCMAKE_INSTALL_PREFIX=/usr/local/)
* make -j 4 [&& make install]

Windows Build Instructions (x64)
--------------------------------

Requires Visual Studio 2015, 2017, or 2019

Tested with the "Visual Studio 15 2017 Win64", "Visual Studio 14 2015", and
"Visual Studio 16 2019" generators.

Checkout repository:

* git clone https://github.com/otgaard/zap

Checkout submodules:

* cd zap
* git submodule init
* git submodule update

Compile GLFW and install in zap/third_party:

* cd third_party/glfw
* mkdir build
* cd build
* cmake .. -DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_THIRD_PARTY] -G "Visual Studio 16 2019"
* (example -DCMAKE_INSTALL_PREFIX=D:\zap\third_party)
* cmake --build . --config Release --target Install

Download Freetype2 for Windows and install in freetype2 folder in third_party.

Compile GLEW (included in zap source code):

* cd ../../glew/build/vc12
* Open glew.sln and build the Win64 Release build

Build zap:

* cd ../..
* mkdir build
* cd build
* cmake .. -DCMAKE_BUILD_TYPE=[Release|Debug] [-DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_INSTALL]] -G "Visual Studio 16 2019"
* cmake --build . --config [Release|Debug] [--target Install]
