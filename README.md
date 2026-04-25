# LuaBind
**LuaBind** is a C++ library for binding Lua and C++, designed for performance and easy integration into game engines.

# Features
* Based on Lua 5.5
* Cross platform 
* C++ API to access and manipulate Lua types (primitives, tables)
* Binding of C++ classes and C-style opaque types
* Support for inheritance
* Support for std containers, pairs, tuples, optional, expected, unique_ptr
* Support for lightweight types (e.g. math classes)
* Support for Lua scripts
* No exceptions
* No RTTI
* Unit tested

# Requirements
* C++ 20

# INSTALLATION
* Clone the repository to a local folder <br/>
  `git clone https://github.com/StefanoLanza/LuaBind.git`
* The library uses [Premake](https://premake.github.io/) to generate projects for Windows or Linux <br/>
  Install `Premake 5.0.0-beta8` or latest
* On Windows
  * Run `premake5.exe vs20xx --with-tests --with-examples` (replace `vs20xx` with your version of Visual Studio, e.g. `vs2022`) <br>
  This command generates a Visual Studio solution
  * Open `build/vs2022/LuaBind.sln`
  * Select a build configuration
  * Build and run the examples and the UnitTest project
* On Linux
  * Run `./premake gmake -cc=gcc --with-tests --with-examples` <br>
  This command generate GNU makefiles
  * Build and run the examples and the UnitTest application
     * `make -C build/gmake config=configuration` (configuration is for example `release_x86`)
* For integration in your own project
  * Add the folders `include, src, external/core, external/lua, external/core` to your project build configuration 
  * Add `include` and `external` to your project include paths
  <br>
  Please see the file premake5.lua as a reference.
  * We will support industry-standard CMake in a future patch to facilitate this process

# CONFIGURATION
Look at the file `include/config.h` Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] CMake support
- [ ] Support for default function arguments
- [ ] Support for MacOS, iOS, Android, HarmonyOS
- [ ] Support for C++ 17 on request
- [ ] Documentation
- [ ] Conan package
