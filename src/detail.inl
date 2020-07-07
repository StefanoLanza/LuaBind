#include "autoBlock.h"
#include "table.h"
#include <cassert>
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

template <typename T>
int createTemporaryObject(lua_State* ls) {
	// Allocate user data in temporary memory and construct new object in that memory
	T* const ud = new (allocTemporary<T>()) T;
	lua_pushlightuserdata(ls, ud);
	return 1;
}

template <class T>
int box(lua_State* ls) {
	void* mem = allocateBoxed(sizeof(T), std::alignment_of_v<T>);
	T*    boxed = new (mem) T;

	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof boxed);
	// Construct new object and store a pointer to its pointer in the user data
	std::memcpy(ud, &boxed, sizeof boxed);

	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                    // mt, ud
	lua_pushvalue(ls, -1);               // mt, mt, ud
	lua_pushcfunction(ls, collectBoxed); // func, mt, mt, ud
	lua_setfield(ls, -2, "__gc");        // mt, mt, ud
	lua_setmetatable(ls, -3);            // mt, ud

	lua_pop(ls, 1); // ud

	return 1;
}

template <class T>
int store(lua_State* ls) {
	// Stack: boxed (ud), value(ud)
	assert(lua_isuserdata(ls, 1));
	assert(lua_isuserdata(ls, 2));

	T* boxed = nullptr;
	std::memcpy(&boxed, lua_touserdata(ls, 1), sizeof boxed);
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
	const T* boxed = nullptr;
	std::memcpy(&boxed, lua_touserdata(ls, 1), sizeof boxed);
	// Retrieve value stored in boxed object
	return Wrapper<T>::Push(ls, *boxed);
}

template <class Class>
void pushBoxingFunctions(lua_State* ls, int tableStackIndex) {
	lua_pushliteral(ls, "Box");
	lua_pushcfunction(ls, box<Class>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "Store");
	lua_pushcfunction(ls, store<Class>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "Retrieve");
	lua_pushcfunction(ls, retrieve<Class>);
	lua_settable(ls, tableStackIndex);
}

template <class T>
void pushObjectAsFullUserData(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const char* const classID = GetClassID(GetTypeInfo(objectPtr));
	pushObjectAsFullUserData(ls, static_cast<void*>(objectPtr), classID);
}

template <typename T>
void addDestructor(lua_State* ls) {
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
	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof(ptr));
	// Store the object pointer in the user data
	std::memcpy(ud, &ptr, sizeof ptr);
	// Push destructor if not trivially destructible
	if constexpr (! std::is_trivially_destructible_v<T>) {
		addDestructor<T>(ls);
		// Set metatable of user data
		// Because the destructor already introduced a mt, we need to nest two mts, 
		// the destructor one and the one associated with the object type

		lua_getmetatable(ls, -1); // mt0
		assert(lua_istable(ls, -1));

		lua_pushliteral(ls, "__index");  // mt0, __index
		luaL_getmetatable(ls, typeName); // mt0, __index, mt1
		lua_pushliteral(ls, "__index");  // mt0, __index, mt1, __index
		lua_rawget(ls, -2);              // mt0, __index, mt1, mt1.t
		lua_remove(ls, -2);              // mt0, __index, mt1.t
		lua_settable(ls, -3);            // mt0                              mt0.__index = mt1.t
		lua_pop(ls, 1);
	}
	else {
		// Set metatable of user data
		luaL_getmetatable(ls, typeName);
		assert(lua_istable(ls, -1)); // mt
		lua_setmetatable(ls, -2);    // ud.mt = mt
	}

#if LUA_TYPE_SAFE
	// TODO in the userdata instead?
	registerPointerType(ptr, getTypeId<T>());
#endif
	return 1;
}

} // namespace Typhoon::LuaBind::detail
