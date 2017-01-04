# zap

The zap engine has as few dependencies as possible.  At present, only GLFW and GLEW are
required.  GLFW is a submodule and can be checked out from Github, GLEW is included 
directly in the third_party folder.

macOS/Linux Build Instructions
------------------------------

Tested on GCC 5.4.0 && Apple LLVM 7.3.0. Please create an issue if you have problems pertaining to building.

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
* cmake .. -DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_INSTALL] -DCMAKE_BUILD_TYPE=Release
* (example -DCMAKE_INSTALL_PREFIX=/usr/local/)

Windows Build Instructions (x64)
--------------------------------

Note: The windows dynamic build is not complete, therefore the engine will be built statically.  This
should be completed soon.

Tested with the "Visual Studio 14 2015 Win64" and "Visual Studio 14 2015" generators.

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
* cmake .. -DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_THIRD_PARTY] -G "Visual Studio 14 2015 Win64"
* (example -DCMAKE_INSTALL_PREFIX=/home/otgaard/zap/third_party/)
* Open GLFW.sln in Visual Studio 2015 and build the Release build

Compile GLEW (included in zap source code):

* cd ../../glew/build/vc12
* Open glew.sln and build the Win64 Release build

Build zap:

* cd ../..
* mkdir build
* cd build
* cmake .. -DCMAKE_INSTALL_PREFIX=[PATH_TO_ZAP_INSTALL] -G "Visual Studio 14 2015 Win64"
* (example -DCMAKE_INSTALL_PREFIX=/usr/local/)
* Open zap.sln and build the Win64 Release build