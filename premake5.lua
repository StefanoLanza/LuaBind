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
buildoptions { "/permissive-", }
system "Windows"

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
	files "thirdParty/lua/src/*.c"
	files "thirdParty/lua/src/*.h"
--	includedirs { "TinyXML", }

project("Core")
	kind "StaticLib"
	files "core/**.cpp"
	files "core/**.h"
	includedirs { "./", }

project("LuaBind")
	kind "StaticLib"
	files "src/**.cpp"
	files "src/**.h"
	files "include/**.h"
	includedirs { "./", "include/", "./thirdParty/", }
	links({"Core", "Lua"})

project("Example1")
	kind "ConsoleApp"
	files "examples/example1.cpp"
	includedirs { "./", "include/", "./thirdParty/", }
	links({"LuaBind", })

project("UnitTest")
	kind "ConsoleApp"
	links({"LuaBind", })
	files "test/*.*"
	includedirs { "./", "include/", "./thirdParty/", }
