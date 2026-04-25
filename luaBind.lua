require "external/core/core"

project "Lua"
	kind "StaticLib"
	files { "external/lua/src/*.c", "external/lua/src/*.h", }
	excludes { "external/lua/src/luac.c", "external/lua/src/lua.c", }
	warnings "Off"


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
