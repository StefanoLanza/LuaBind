#include "objectRegistration.h"
#include "autoBlock.h"
#include "stackUtils.h"
#include "table.h"
#include "typeSafefy.h"
#include <cassert>

namespace Typhoon::LuaBind {

Reference registerObjectAsTable(lua_State* ls, void* objectPtr, TypeId typeId) {
	assert(objectPtr);

	AutoBlock autoBlock(ls);

	const TypeName className = typeIdToName(typeId);

	// Lookup class metatable in registry
	luaL_getmetatable(ls, className);
	if (! lua_istable(ls, -1)) {
		return Reference {}; // class not registered
	}
	const int mti = lua_gettop(ls);

	// Create a table representing the object
	lua_newtable(ls);
	lua_pushvalue(ls, mti);
	lua_setmetatable(ls, -2);
	const int tableStackIndex = lua_gettop(ls);

	// table["_ptr"] = objectPtr
	lua_pushstring(ls, "_ptr");
	lua_pushlightuserdata(ls, objectPtr);
	lua_rawset(ls, tableStackIndex);

	// table["_typeId"] = typeId
	lua_pushstring(ls, "_typeid");
	lua_pushlightuserdata(ls, const_cast<void*>(typeId.impl));
	lua_rawset(ls, tableStackIndex);

	// Cache association in registry
	const lua_Integer ptrKey = detail::makePointerKey(objectPtr, typeId);
	// registry[objectPtr] = table
	lua_pushinteger(ls, ptrKey);
	lua_pushvalue(ls, tableStackIndex);
	lua_rawset(ls, LUA_REGISTRYINDEX);

	// Create reference to table and save it in the registry
	lua_pushvalue(ls, tableStackIndex);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

Reference registerObjectAsUserData(lua_State* ls, void* objectPtr, TypeId typeId) {
	assert(objectPtr);

	AutoBlock autoBlock(ls);

	const TypeName typeName = typeIdToName(typeId);
	if (! typeName) {
		return Reference {}; // class not registered in C++
	}

	void* const userData = lua_newuserdatauv(ls, sizeof objectPtr, 1);
	// Copy C++ pointer to Lua userdata
	std::memcpy(userData, &objectPtr, sizeof objectPtr);
	const int userDataIndex = lua_gettop(ls);

	// Lookup class metatable in registry
	luaL_getmetatable(ls, typeName);
	if (! lua_istable(ls, -1)) {
		return Reference {}; // class not registered
	}
	// Set metatable of user data at index idx
	lua_setmetatable(ls, userDataIndex);

	lua_pushinteger(ls, typeId.value());
	lua_setiuservalue(ls, userDataIndex, 1); // ud.userValue[1] = typeId

	// Save association objectPtr -> user data in registry, used when pushing C++ pointers on the Lua stack
	// registry[{objectPtr, typeId}] = userData
	const lua_Integer ptrKey = detail::makePointerKey(objectPtr, typeId);
	lua_pushinteger(ls, ptrKey);
	lua_pushvalue(ls, userDataIndex);
	lua_rawset(ls, LUA_REGISTRYINDEX);

	// Create reference to user data
	lua_pushvalue(ls, userDataIndex);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

Reference registerObjectAsLightUserData(lua_State* ls, void* objectPtr, [[maybe_unused]] TypeId typeId) {
	assert(objectPtr);

#if TY_LUABIND_TYPE_SAFE
	//TODO detail::registerPointer(ls, objectPtr, typeId);
#endif

	AutoBlock autoBlock(ls);
	// Create reference to user data
	lua_pushlightuserdata(ls, objectPtr);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

Reference registerTable(lua_State* ls, const char* className) {
	assert(className);
	AutoBlock autoBlock(ls);

	// Lookup class metatable in registry
	luaL_getmetatable(ls, className);
	if (! lua_istable(ls, -1)) {
		return Reference {};
	}
	const int mti = lua_gettop(ls);

	// Create a table representing the object
	lua_newtable(ls);
	lua_pushvalue(ls, mti);
	lua_setmetatable(ls, -2);
	const int tableStackIndex = lua_gettop(ls);

	// Call constructor (might be nil)
	lua_getfield(ls, tableStackIndex, "__constructor");
	if (lua_isfunction(ls, -1)) {
		lua_pushvalue(ls, tableStackIndex);
		const int lres = lua_pcall(ls, 1, 0, 0);
		if (0 != lres) {
			// Constructor failed
			return Reference {};
		}
	}

	// Create reference to table
	lua_pushvalue(ls, tableStackIndex);
	const int ref = luaL_ref(ls, LUA_REGISTRYINDEX);
	return Reference { ref };
}

void unregisterTable(lua_State* ls, Reference ref) {
	luaL_unref(ls, LUA_REGISTRYINDEX, ref.getValue());
}

void unregisterObject(lua_State* ls, Reference ref) {
	assert(ref.isValid());
	AutoBlock autoBlock(ls);

	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref.getValue());
	const int objectIndex = lua_gettop(ls);
	const int luaType = lua_type(ls, objectIndex);
	// Retrieve object pointer
	void* objectPtr = nullptr;
	if (luaType == LUA_TUSERDATA) {
		void* userData = lua_touserdata(ls, objectIndex);
		std::memcpy(&objectPtr, userData, sizeof objectPtr);
	}
	else if (luaType == LUA_TLIGHTUSERDATA) {
		objectPtr = lua_touserdata(ls, objectIndex);
	}
	else if (luaType == LUA_TTABLE) {
		lua_pushstring(ls, "_ptr");
		lua_rawget(ls, objectIndex);
		objectPtr = lua_touserdata(ls, -1);

		// Set _ptr to nil. Lua objects can check if a C++ object is still alive this way
		// table["_ptr"] = nil
		lua_pushstring(ls, "_ptr");
		lua_pushnil(ls);
		lua_rawset(ls, objectIndex);
	}
	else {
		// Not an object
	}

	// registry[objectPtr] = nil
	if (objectPtr) {
		lua_pushlightuserdata(ls, objectPtr);
		lua_pushnil(ls);
		lua_rawset(ls, LUA_REGISTRYINDEX);
#if TY_LUABIND_TYPE_SAFE
		// FIXME detail::unregisterPointer(ls, objectPtr);
#endif
	}

	// Delete reference
	luaL_unref(ls, LUA_REGISTRYINDEX, ref.getValue());
}

void unregisterObject(lua_State* ls, void* objectPtr, TypeId typeId) {
	if (! objectPtr) {
		return;
	}
	AutoBlock autoBlock(ls);

	const lua_Integer ptrKey = detail::makePointerKey(objectPtr, typeId);

	lua_pushinteger(ls, ptrKey);
	lua_rawget(ls, LUA_REGISTRYINDEX);
	const int objectIndex = lua_gettop(ls);
	const int luaType = lua_type(ls, objectIndex);
	assert(luaType != LUA_TNIL);
	if (luaType == LUA_TTABLE) {
		// Set _ptr to nil. Lua objects can check if a C++ object is still alive this way
		// table["_ptr"] = nil
		lua_pushstring(ls, "_ptr");
		lua_pushnil(ls);
		lua_rawset(ls, objectIndex);
	}

	// registry[{objectPtr, typeId}] = nil
	lua_pushinteger(ls, ptrKey);
	lua_pushnil(ls);
	lua_rawset(ls, LUA_REGISTRYINDEX);

#if TY_LUABIND_TYPE_SAFE
//	detail::unregisterPointer(ls, objectPtr, typeId);
#endif
}

} // namespace Typhoon::LuaBind
