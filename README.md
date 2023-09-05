**Typhoon LuaBind** is a C++ library for binding Lua and C++.

# Features
* Binding of C++ classes and structs.
* Object oriented C++ API to access Lua types (primitives, tables)
* Support for lightweight types (e.g. math primitives)
* No exceptions
* No RTTI

# Requirements
* C++ 17

# INSTALLATION
* Clone the repository to a local folder
  git clone https://github.com/StefanoLanza/LuaBind.git
* Reflection uses [Premake](https://premake.github.io/) to generate projects for Windows, Linux and MacOS
* On Windows
  * Run `premake5.exe vs20xx --with-tests --with-examples` (replace `vs20xx` with your version of Visual Studio, e.g. `vs2017`, `vs2019` or `vs2022`) <br>
  This command generates a Visual Studio solution
  * Open build/vs2022/Typhoon-LuaBind.sln
  * Select a build configuration
  * Build and run the examples and the UnitTest project
* On Linux
  * Run `premake gmake -cc=gcc --with-tests --with-examples`. <br>
  This command generate GNU makefiles
  * Build and run the examples and the UnitTest application
     * `make -C build/gmake config=configuration` (e.g. `release_x86`)
* On MacOS
  * Run `premake xcode4` <br>
  This command generates an XCode workspace
  * Open build/xcode4/Typhoon-LuaBind.xcworkspace
  * Select a scheme under Product/Scheme/
  * Build and run the examples and the UnitTest scheme
* For integration in your own project
  * Add the folders include, src, external/core, external/lua to your project build configuration 
  * Add `include` and `external` to your project include paths
  <br>
  Please see the file premake5.lua as a reference.

# CONFIGURATION
Look at the file include/config.h Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] Support for default function arguments
- [ ] Documentation
- [ ] Conan package
- [ ] Support for Lua 5.4.3

# CONTRIBUTE
