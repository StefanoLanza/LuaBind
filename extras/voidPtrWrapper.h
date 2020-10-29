#pragma once

#include "../include/typeWrapper.h"
#include <core/typedVoidPtr.h>

namespace Typhoon::LuaBind {

template <>
class Wrapper<VoidPtr> {
public:
	static int Match(lua_State* ls, int idx) {
		return Wrapper<void*>::Match(ls, idx);
	}
	static int PushAsKey(lua_State* ls, VoidPtr voidPtr) {
		return Wrapper<void*>::PushAsKey(ls, voidPtr.ptr);
	}
	static int Push(lua_State* ls, VoidPtr voidPtr) {
		if (voidPtr.ptr == nullptr) {
			return 0;
		}

		const TypeName typeName = typeIdToName(voidPtr.typeId);
		if (! typeName) {
			// luaL_error(ls, "class not registered");
			return 0;
		}

		// FIXME Allocate from temporary pool ?
		// Copy C++ pointer to Lua userdata
		void* const ptr = voidPtr.ptr;
		void* const userData = lua_newuserdata(ls, sizeof ptr);
		std::memcpy(userData, &ptr, sizeof ptr);
		const int userDataIndex = lua_gettop(ls);

		// Lookup class metatable in registry
		luaL_getmetatable(ls, typeName);
		assert(lua_istable(ls, -1));
		// Set metatable of user data at index idx
		lua_setmetatable(ls, userDataIndex);
		return 1;
	}
	/*	static int Get(lua_State* ls, int idx, VoidPtr& voidPtr)
	    {
	        return LUA::Get(ls, idx, &ref);
	    }*/
	static constexpr int stackSize = 1;
};

} // namespace Typhoon::LuaBind
