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
	// Delete object
	delete obj;
	return 0;
}

template <class T>
int box(lua_State* ls) {
	void* mem = allocateBoxed(sizeof(T), std::alignment_of_v<T>);
	T*    boxed = new (mem) T;

	// Optionally initialize the boxed object
	if (const T* value = static_cast<const T*>(lua_touserdata(ls, 1)); value) {
		*boxed = *value;
	}

	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof boxed);
	// Construct new object and store a pointer to its pointer in the user data
	std::memcpy(ud, &boxed, sizeof boxed);

	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                    // ud, mt
	lua_pushvalue(ls, -1);               // ud, mt, mt
	lua_pushcfunction(ls, collectBoxed); // ud, mt, mt, collectBoxes
	lua_setfield(ls, -2, "__gc");        // ud, mt, mt
	lua_setmetatable(ls, -3);            // ud, mt

	lua_pop(ls, 1); // ud

	return 1;
}

template <class T>
int store(lua_State* ls) {
	// Stack: boxed (ud), value(ud)
	assert(lua_isuserdata(ls, 1));
	assert(lua_isuserdata(ls, 2));
	// Extract pointer from userdata
	T* boxed = serializePOD<T*>(lua_touserdata(ls, 1));
	// Store value in boxed object
	*boxed = Wrapper<T>::Get(ls, 2);
	return 0;
}

template <class T>
int retrieve(lua_State* ls) {
	// Stack: boxed (ud)
	if (! lua_isuserdata(ls, 1)) {
		return 0;
	}
	// Extract pointer from userdata
	const T* boxed = serializePOD<T*>(lua_touserdata(ls, 1));
	// Push boxed object
	return Wrapper<T>::Push(ls, *boxed);
}

template <class T>
void pushBoxingFunctions(lua_State* ls, int tableStackIndex) {
	static_assert(std::is_trivially_destructible_v<T>, "Type must be trivially destructible");

	lua_pushliteral(ls, "box");
	lua_pushcfunction(ls, box<T>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "store");
	lua_pushcfunction(ls, store<T>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "retrieve");
	lua_pushcfunction(ls, retrieve<T>);
	lua_settable(ls, tableStackIndex);
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
	lua_pushcfunction(ls, garbageCollect<T>); // ud, mt, func
	lua_setfield(ls, -2, "__gc");             // ud, mt          mt[__gc] = func
	lua_setmetatable(ls, -2);                 // ud              ud.mt = mt
}

template <typename T>
int newObject(lua_State* ls) {
	const auto     typeId = getTypeId<T>();
	const TypeName typeName = typeIdToName(typeId);

	T* const ptr = new T;

	// Allocate full user data and store the object pointer in it
	void* const ud = lua_newuserdata(ls, sizeof ptr);
	std::memcpy(ud, &ptr, sizeof ptr);

	// Push destructor if not trivially destructible
	if constexpr (! std::is_trivially_destructible_v<T>) {
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
	else {
		// Set metatable of user data
		luaL_getmetatable(ls, typeName); // ud, mt
		assert(lua_istable(ls, -1));     //
		lua_setmetatable(ls, -2);        // ud       ud.mt = mt
	}

#if LUA_TYPE_SAFE
	// TODO in the userdata instead?
	registerPointerType(ptr, getTypeId<T>());
#endif
	return 1;
}

} // namespace Typhoon::LuaBind::detail
