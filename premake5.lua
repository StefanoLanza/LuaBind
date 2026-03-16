-- Options
newoption {
	trigger     = "with-tests",
	description = "Build the unit test application",
}

newoption {
	trigger     = "with-examples",
	description = "Build the examples",
}

-- Global settings
local workspacePath = path.join("build/", _ACTION)  -- e.g. build/vs2022

-- Filters
local filter_gcc = "toolset:gcc"
local filter_clang = "toolset:clang"
local filter_windows = "system:windows"
local filter_msvc = "toolset:msc*"
local filter_gmake = "action:gmake*"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("LuaBind")
configurations { "Debug", "Release" }
platforms { "x86", "x64" }
language "C++"
cppdialect "c++20"
location (workspacePath)
characterset "MBCS"
enablepch "Off"
multiprocessorcompile("on")
manifest("off")
exceptionhandling "Off"
rtti "Off"
startproject "UnitTest"
defines { "TY_LUABIND_TYPE_SAFE=1", }

-- A small helper to rename x86_64 to x64
function get_arch()
    return "%{cfg.architecture == 'x86_64' and 'x64' or cfg.architecture}"
end

-- Binaries e.g. build/vs2022/bin/x64/Release/UnitTest.exe
targetdir (workspacePath .. "/bin/" .. get_arch() .. "/%{cfg.buildcfg}")

filter { filter_msvc }
	buildoptions
	{
		"/permissive-",
		"/Zc:preprocessor",  -- support for __VA_OPT__ in C++20
		"/Zc:__cplusplus",   -- __cplusplus will now report 202002L (for C++20)
	}
	system "Windows"
	defines {
		"_HAS_EXCEPTIONS=0",	
		"_ENABLE_EXTENDED_ALIGNED_STORAGE", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", 
	}

filter { filter_x86 }
	architecture "x86"
	  
filter { filter_x64 }
	architecture "x86_64"
	
filter { filter_windows, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_windows, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_clang, filter_debug, }
	defines { }
	-- Address sanitizer for clang
	buildoptions
	{
		"/fsanitize=address",
	}
	-- Turn off incompatible options
	incrementallink "Off"
	runtimechecks "Off"
	editAndContinue "Off"

filter { filter_msvc, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", }

filter { filter_gcc }
-- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
	buildoptions { "-ffat-lto-objects" }

filter { filter_debug }
	defines { "_DEBUG", "DEBUG", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { filter_release }
	defines { "NDEBUG", }
	buffersecuritycheck "off"
	runtimechecks "Off"
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"
	linktimeoptimization "On"

filter {} -- clear filters

project "Lua"
	kind "StaticLib"
	files { "external/lua/src/*.c", "external/lua/src/*.h", }
	excludes { "external/lua/src/luac.c", "external/lua/src/lua.c", }
	warnings "Off"

require "external/core/core"

project "LuaBind"
	kind "StaticLib"
	files { "src/**.cpp", "src/**.h", "src/**.inl", "include/luaBind/**.h", "include/luaBind/**.inl", }
	includedirs { "./", "external", "include/luaBind", }
	uses({"Core", })
	links {"Lua", }
	usage "INTERFACE"
		uses {"Core" } -- propagate to customers
		includedirs { "include" }
		includedirs { "external" } -- Lua
		links {"LuaBind" }

if _OPTIONS["with-examples"] then
	project "Example1"
		kind "ConsoleApp"
		files "examples/example1.cpp"
		uses {"LuaBind", }
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project "Example2"
		kind "ConsoleApp"
		files "examples/example2.cpp"
		uses {"LuaBind", }
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project "Example3"
		kind "ConsoleApp"
		files "examples/example3.cpp"
		uses {"LuaBind", }
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project "Example4"
		kind "ConsoleApp"
		files "examples/example4.cpp"
		uses {"LuaBind", }
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}
end

if _OPTIONS["with-tests"] then
		project("Catch")
		kind "StaticLib"
		files { "external/Catch/*.cpp", "external", "external/Catch/*.hpp", } 
		includedirs { "external/Catch", }	
		project("UnitTest")
		kind "ConsoleApp"
		files "test/*.*"
		links({"Catch",})
		uses({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}
end
