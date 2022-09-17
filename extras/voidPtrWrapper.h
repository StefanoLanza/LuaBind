#pragma once

#include <luaBind/typeWrapper.h>
#include <core/typedVoidPtr.h>

namespace Typhoon::LuaBind {

template <>
class Wrapper<VoidPtr> {
public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return Wrapper<void*>::match(ls, idx);
	}

	static void pushAsKey(lua_State* ls, VoidPtr voidPtr) {
		Wrapper<void*>::pushAsKey(ls, voidPtr.ptr);
	}

	static void push(lua_State* ls, VoidPtr voidPtr) {
		if (voidPtr.ptr == nullptr) {
			lua_pushnil(ls);
			return;
		}

		const TypeName typeName = typeIdToName(voidPtr.typeId);
		if (! typeName) {
			luaL_error(ls, "class not registered");
		}

		// Copy C++ pointer to Lua userdata
		void* const ptr = voidPtr.ptr;
		void* const userData = lua_newuserdatauv(ls, sizeof ptr, 0);
		std::memcpy(userData, &ptr, sizeof ptr);
		const int userDataIndex = lua_gettop(ls);

#if TY_LUABIND_TYPE_SAFE
		detail::registerPointer(ls, voidPtr.ptr, voidPtr.typeId);
#endif

		// Lookup class metatable in registry
		luaL_getmetatable(ls, typeName);
		assert(lua_istable(ls, -1));
		// Set metatable of user data at index idx
		lua_setmetatable(ls, userDataIndex);
	}
	// pop not supported
};

} // namespace Typhoon::LuaBind
