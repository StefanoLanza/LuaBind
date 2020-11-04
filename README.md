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
* LuaBind uses Premake to generate projects for Windows and MacOS. 
* On Windows
  * Run ```premake5.exe vsXXXX``` to generate a Visual Studio XXXX (either vs2017 or 2019) solution
  * Open build/vs2019/Typhoon-LuaBind.sln
  * Select a build configuration (Release, Debug, Win32, x64)
  * Build and run the examples and the UnitTest project
* On MacOS
  * Run ```premake xcode4``` to generate an XCode workspace
  * Open build/xcode4/Typhoon-LuaBind.xcworkspace
  * Select a scheme under Product/Scheme/
  * Build and run the examples and the UnitTest scheme
* For integration in your own project
  * Add the folders include, src, external/core, external/lua to your project build pipeline. Please see the build configuration in premake5.lua as a reference.

# CONFIGURATION
Look at the file include/config.h Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] Type safety
- [ ] Support for default function arguments
- [ ] Documentation

# CONTRIBUTE
