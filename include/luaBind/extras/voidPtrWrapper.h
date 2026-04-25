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

		// TODO Query isLightweight(voidPtr.typeId) at runtime
#if 0
		if (isLightweight(voidPtr.typeId)) {
			// lightweight type, push ptr as light user data
			lua_pushlightuserdata(ls, ptr);
#if TY_LUABIND_TYPE_SAFE
			detail::registerTemporaryPointer(ls, ptr, voidPtr.typeId);
#endif
			return;
		}
#endif

		// Try getting userdata associated with the pointer from registry
		const lua_Integer ptrKey = detail::makePointerKey(voidPtr.ptr, voidPtr.typeId);
		lua_pushinteger(ls, ptrKey);
		lua_rawget(ls, LUA_REGISTRYINDEX);
		if (lua_isnil(ls, -1)) {
			const TypeName typeName = typeIdToName(voidPtr.typeId);
			if (! typeName) {
				// Unregistered class voidPtr.typeId, push ptr as light user data
				lua_pushlightuserdata(ls, voidPtr.ptr);
				return;
			}

			// Copy C++ pointer to Lua userdata
			void* const ptr = voidPtr.ptr;
			void* const userData = lua_newuserdatauv(ls, sizeof ptr, 1);
			std::memcpy(userData, &ptr, sizeof ptr);
			const int userDataIndex = lua_gettop(ls);

			// Lookup class metatable in registry
			luaL_getmetatable(ls, typeName);
			assert(lua_istable(ls, -1));
			// Set metatable of user data at index idx
			lua_setmetatable(ls, userDataIndex);

			lua_pushinteger(ls, voidPtr.typeId.value());
			lua_setiuservalue(ls, -2, 1); // ud.userValue[1] = typeId

			// Cache user data in registry
			lua_pushinteger(ls, ptrKey);
			lua_pushvalue(ls, userDataIndex);
			lua_rawset(ls, LUA_REGISTRYINDEX);
		}
		else {
			// The ptr has been registered. Return it
		}
	}
	// pop not supported
};

} // namespace Typhoon::LuaBind
