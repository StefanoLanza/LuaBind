#include "autoBlock.h"
#include "table.h"
#include <cassert>
#include <core/ptrUtil.h>
#include <core/typeId.h>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

template <class T>
int garbageCollect(lua_State* ls) {
	// Extract pointer from user data
	T* obj = nullptr;
	std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
	assert(obj);

	delete obj;
	return 0;
}

template <class T>
void pushObjectAsFullUserData(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const char* const classID = GetClassID(GetTypeInfo(objectPtr));
	pushObjectAsFullUserData(ls, static_cast<void*>(objectPtr), classID);
}

template <typename T>
void setDestructor(lua_State* ls) {
	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                         // ud, mt
	lua_pushcfunction(ls, garbageCollect<T>); // ud, mt, gc
	lua_setfield(ls, -2, "__gc");             // ud, mt          mt[__gc] = gc
	lua_setmetatable(ls, -2);                 // ud              ud.mt = mt
}

template <typename T>
int newObject(lua_State* ls) {
	const auto     typeId = getTypeId<T>();
	const TypeName typeName = typeIdToName(typeId);

	const auto ptr = new T;

	// Allocate full user data and store the object pointer in it
	void* const ud = lua_newuserdata(ls, sizeof ptr);
	std::memcpy(ud, &ptr, sizeof ptr);

	// Push destructor if not trivially destructible
	if constexpr (std::is_trivially_destructible_v<T>) {
		// Set metatable of user data
		luaL_getmetatable(ls, typeName); // ud, mt
		assert(lua_istable(ls, -1));
		lua_setmetatable(ls, -2);        // ud       ud.mt = mt
	}
	else {
		// See comment in registerCppClass for a possible alternative...
		setDestructor<T>(ls);
		// Set metatable of user data
		// Because the destructor already introduced a mt, we need to nest two mts,
		// the destructor one (mt0) and the one associated with the object type (mt1)
		lua_getmetatable(ls, -1);        // ud, mt0
		lua_pushliteral(ls, "__index");  // ud, mt0, __index
		luaL_getmetatable(ls, typeName); // ud, mt0, __index, mt1
		lua_settable(ls, -3);            // ud, mt0                    mt0.__index = mt1
		lua_pop(ls, 1);
	}

#if TY_LUABIND_TYPE_SAFE
	registerPointer(ls, ptr);
#endif
	return 1;
}

} // namespace Typhoon::LuaBind::detail
