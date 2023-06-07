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
local workspacePath = path.join("build/", _ACTION)  -- e.g. build/vs2019

-- Filters
local filter_vs = "action:vs*"
local filter_gcc = "toolset:gcc"
local filter_clang = "toolset:clang"
local filter_xcode = "action:xcode*"
local filter_gmake = "action:gmake*"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x86_64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("Typhoon-LuaBind")
configurations { "Debug", "Release" }
platforms { "x86", "x86_64" }
language "C++"
location (workspacePath)
characterset "MBCS"
flags   { "MultiProcessorCompile", }
startproject "UnitTest"
exceptionhandling "Off"
defines { "TY_LUABIND_TYPE_SAFE=1", }
cppdialect "c++17"
rtti "Off"

filter { filter_vs }
	buildoptions
	{
		"/permissive-",
		"/Zc:preprocessor",  -- support for __VA_OPT__ in C++20
	}
	system "Windows"
	defines {
		"_HAS_EXCEPTIONS=0",	
		"_ENABLE_EXTENDED_ALIGNED_STORAGE", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", 
	}

filter { filter_xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter { filter_x86 }
	architecture "x86"
	  
filter { filter_x64 }
	architecture "x86_64"
	
filter { filter_vs }

filter { filter_vs, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_vs, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_clang, filter_debug, }
	defines { }
	-- Address sanitizer for clang
	buildoptions
	{
		"/fsanitize=address",
	}
	-- Turn off incompatible options
	flags { "NoIncrementalLink", "NoRuntimeChecks", }
	editAndContinue "Off"

filter { filter_vs, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", }

filter { filter_gcc }
-- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
	buildoptions { "-ffat-lto-objects" }

filter { filter_debug }
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { filter_release }
	defines { "NDEBUG", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

filter {}

project("Lua")
	kind "StaticLib"
	files { "external/lua/src/*.c", "external/lua/src/*.h", }
	excludes { "external/lua/src/luac.c", "external/lua/src/lua.c", } 

project("Core")
	kind "StaticLib"
	files { "external/core/**.cpp", "external/core/**.h", }
	externalincludedirs { "./", "external", }

project("LuaBind")
	kind "StaticLib"
	files { "src/**.cpp", "src/**.h", "src/**.inl", "include/luaBind/**.h", "include/luaBind/**.inl", }
	externalincludedirs { "./", "external", "include/luaBind", }
	links({"Core", "Lua"})

if _OPTIONS["with-examples"] then
	project("Example1")
		kind "ConsoleApp"
		files "examples/example1.cpp"
		externalincludedirs { "./", "external", "include", }
		links({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project("Example2")
		kind "ConsoleApp"
		files "examples/example2.cpp"
		externalincludedirs { "./", "external", "include", }
		links({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project("Example3")
		kind "ConsoleApp"
		files "examples/example3.cpp"
		externalincludedirs { "./", "external", "include", }
		links({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}

	project("Example4")
		kind "ConsoleApp"
		files "examples/example4.cpp"
		externalincludedirs { "./", "external", "include", }
		links({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}
end

if _OPTIONS["with-tests"] then
	project("UnitTest")
		kind "ConsoleApp"
		files "test/*.*"
		externalincludedirs { "./", "external", "include", }
		links({"LuaBind", })
		filter { filter_gmake }
			links({"Core", "Lua"})
		filter {}
end
