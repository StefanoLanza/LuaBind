#pragma once

#include <core/typedVoidPtr.h>
#include <luaBind/typeWrapper.h>

namespace Typhoon::LuaBind {

template <>
class Wrapper<VoidPtr> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
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

		void* const ptrKey = detail::makePointerKey(voidPtr.ptr, voidPtr.typeId);
		// Get userdata/table associated with the pointer from registry
		lua_pushlightuserdata(ls, ptrKey);
		lua_rawget(ls, LUA_REGISTRYINDEX);
		if (lua_isnil(ls, -1)) {

			const TypeName typeName = typeIdToName(voidPtr.typeId);
			if (! typeName) {
				luaL_error(ls, "Failed to push instance of VoidPtr. C++ class not registered");
			}

			// Copy C++ pointer to Lua userdata
			void* const ptr = voidPtr.ptr;
			void* const userData = lua_newuserdatauv(ls, sizeof ptr, 0);
			std::memcpy(userData, &ptr, sizeof ptr);
			const int userDataIndex = lua_gettop(ls);

			// Lookup class metatable in registry
			luaL_getmetatable(ls, typeName);
			assert(lua_istable(ls, -1));
			// Set metatable of user data at index idx
			lua_setmetatable(ls, userDataIndex);

			// Cache association ptr -> user data in registry
			lua_pushlightuserdata(ls, ptrKey);
			lua_pushvalue(ls, userDataIndex);
			lua_rawset(ls, LUA_REGISTRYINDEX);

#if TY_LUABIND_TYPE_SAFE
			detail::registerPointer(ls, voidPtr.ptr, voidPtr.typeId);
#endif
		}
		else {
			// The ptr has been registered. Return it
		}
	}
	// pop not supported
};

} // namespace Typhoon::LuaBind
