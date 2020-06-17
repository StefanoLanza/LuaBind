**Typhoon Lua** is a C++ library for binding Lua and C++

# Features
* No exceptions
* No RTTI

# Requirements
* C++ 17

# INSTALLATION
* Clone the repository to a local folder
* For testing
  * Run ```premake5.exe vs2019``` to generate a Visual Studio 2019 solution
  * Select a build configuration (Release, Debug, Win32, x64)
  * Build and run the UnitTest project
* For integration in your own project
  * Add the folders core, reflection and TinyXML to your project build pipeline.

# CONFIGURATION
Look at the file src/config.h Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] Type safety
- [ ] Documentation

# CONTRIBUTE
