-- Options
newoption {
	trigger     = "with-tests",
	description = "Build the unit test application",
}

newoption {
	trigger     = "with-examples",
	description = "Build the examples",
}

newoption {
	trigger     = "with-asan",
	description = "Enable address sanitizer",
}

-- Global settings
local workspacePath = path.join("build/", _ACTION)  -- e.g. build/vs2019

-- Filters
local vs = "action:vs*"
local xcode = "action:xcode*"
local x86 = "platforms:x86"
local x64 = "platforms:x86_64"
local debug =  "configurations:Debug*"
local release =  "configurations:Release*"

-- Address sanitizer
local asan = _OPTIONS["with-asan"]
if (not (_ACTION == "vs2019")) then
	print ("Address Sanitizer not supported")
	asan = false
end

workspace ("Typhoon-LuaBind")
configurations { "Debug", "Release" }
platforms { "x86", "x86_64" }
language "C++"
location (workspacePath)
characterset "MBCS"
flags   { "MultiProcessorCompile", }
startproject "UnitTest"
exceptionhandling "Off"
defines { "_HAS_EXCEPTIONS=0", "TY_LUABIND_TYPE_SAFE=1", }
cppdialect "c++17"
rtti "Off"

filter { vs }
	buildoptions
	{
		"/permissive-",
	}
	system "Windows"
	defines { 
		"_ENABLE_EXTENDED_ALIGNED_STORAGE", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", 
		"_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", 
	}

filter { xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter { x86 }
	architecture "x86"
	  
filter { x64 }
	architecture "x86_64"
	
filter { vs }

filter { vs, x86, }
	defines { "WIN32", "_WIN32", }

filter { vs, x64, }
	defines { "WIN64", "_WIN64", }

filter { vs, debug, }
	defines { }
	-- Address sanitizer for VS 2019
	if asan then 
		buildoptions
		{
			"/fsanitize=address",
		}
		-- Turn off incompatible options
		flags { "NoIncrementalLink", "NoRuntimeChecks", }
		editAndContinue "Off"
	end

filter { vs, release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", }

filter { debug }
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { release }
	defines { "NDEBUG", }
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
	files { "src/**.cpp", "src/**.h", "src/**.inl", "include/LuaBind/**.h", "include/LuaBind/**.inl", }
	sysincludedirs { "./", "external", "include/LuaBind", }
	links({"Core", "Lua"})

if _OPTIONS["with-examples"] then
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
end

if _OPTIONS["with-tests"] then
	project("UnitTest")
		kind "ConsoleApp"
		links({"LuaBind", })
		files "test/*.*"
		sysincludedirs { "./", "external", "include", }
end