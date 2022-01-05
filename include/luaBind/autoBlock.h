#pragma once

#include <core/uncopyable.h>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind {

// Helper class to automatically restore the Lua stack when a function exits
class AutoBlock : Uncopyable {
public:
	explicit AutoBlock(lua_State* ls)
	    : ls(ls)
	    , stackTop(lua_gettop(ls)) {
	}

	~AutoBlock() {
		lua_settop(ls, stackTop);
	}

	// For debugging
	int getStackSize() const {
		return lua_gettop(ls) - stackTop;
	}

private:
	lua_State* ls;
	int        stackTop;
};

} // namespace Typhoon::LuaBind
