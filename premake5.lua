-- Global settings
local workspacePath = path.join("build/", _ACTION)  -- e.g. build/vs2019

workspace ("Typhoon-LuaBind")
configurations { "Debug", "Release" }
platforms { "x86", "x86_64" }
language "C++"
location (workspacePath)
characterset "MBCS"
flags   { "MultiProcessorCompile", }
startproject "UnitTest"
exceptionhandling "Off"
defines { "_HAS_EXCEPTIONS=0" }
cppdialect "c++17"
rtti "Off"

filter "action:vs*"
	buildoptions
	{
		"/permissive-",
	}
	system "Windows"
	-- systemversion "10.0.17134.0"

filter "action:xcode*"
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter "platforms:x86"
	architecture "x86"
	defines { "WIN32", "_WIN32", }
	  
filter "platforms:x86_64"
	architecture "x86_64"
	defines { "WIN64", "_WIN64", }

filter "configurations:Debug*"
	defines { "_DEBUG", "DEBUG", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter "configurations:Release*"
	defines { "NDEBUG", "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

project("Lua")
	kind "StaticLib"
	files { "external/lua/src/*.c", "external/lua/src/*.h", }
	excludes { "external/lua/src/luac.c", } 

project("Core")
	kind "StaticLib"
	files { "external/core/**.cpp", "external/core/**.h", }
	sysincludedirs { "./", "external", }

project("LuaBind")
	kind "StaticLib"
	files { "src/**.cpp", "src/**.h", "src/**.inl", "include/**.h", }
	sysincludedirs { "./", "external", "include", }
	links({"Core", "Lua"})

project("Example1")
	kind "ConsoleApp"
	files "examples/example1.cpp"
	sysincludedirs { "./", "external", "include", }
	links({"LuaBind", })

project("Example2")
	kind "ConsoleApp"
	files "examples/example2.cpp"
	sysincludedirs { "./", "external", "include", }
	links({"LuaBind", })

project("Example3")
	kind "ConsoleApp"
	files "examples/example3.cpp"
	sysincludedirs { "./", "external", "include", }
	links({"LuaBind", })

project("Example4")
	kind "ConsoleApp"
	files "examples/example4.cpp"
	sysincludedirs { "./", "external", "include", }
	links({"LuaBind", })

project("UnitTest")
	kind "ConsoleApp"
	links({"LuaBind", })
	files "test/*.*"
	sysincludedirs { "./", "external", "include", }
