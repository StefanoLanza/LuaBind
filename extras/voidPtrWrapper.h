#pragma once

#include <LuaBind/typeWrapper.h>
#include <core/typedVoidPtr.h>

namespace Typhoon::LuaBind {

template <>
class Wrapper<VoidPtr> {
public:
	static constexpr int stackSize = 1;
	static constexpr bool constRefAsValue = true;

	static int match(lua_State* ls, int idx) {
		return Wrapper<void*>::match(ls, idx);
	}

	static int pushAsKey(lua_State* ls, VoidPtr voidPtr) {
		return Wrapper<void*>::pushAsKey(ls, voidPtr.ptr);
	}

	static int push(lua_State* ls, VoidPtr voidPtr) {
		if (voidPtr.ptr == nullptr) {
			return 0;
		}

		const TypeName typeName = typeIdToName(voidPtr.typeId);
		if (! typeName) {
			return luaL_error(ls, "class not registered");
		}

		// Copy C++ pointer to Lua userdata
		void* const ptr = voidPtr.ptr;
		void* const userData = lua_newuserdata(ls, sizeof ptr);
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
		return 1;
	}
	// pop not supported
};

} // namespace Typhoon::LuaBind
