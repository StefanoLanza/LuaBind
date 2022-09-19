#include "class.h"
#include "autoBlock.h"
#include "context.h"
#include "detail.h"
#include "table.h"
#include <cassert>
#include <unordered_map>

namespace Typhoon::LuaBind::detail {

namespace {

// Store method table in globals so that scripts can add functions written in Lua.
void registerClassInGlobals(lua_State* ls, const char* className, int methodsIndex) {
	AutoBlock autoBlock(ls);

	// globals[classname] = methodsIndex
	lua_rawgeti(ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	lua_pushstring(ls, className);
	lua_pushvalue(ls, methodsIndex);
	lua_settable(ls, -3);
}

} // namespace

Reference registerCppClass(lua_State* ls, const char* className, TypeId classID, TypeId baseClassID) {
	assert(className);
	assert(classID != nullTypeId);

	AutoBlock autoBlock(ls);

	// Create a metatable in the registry
	if (luaL_newmetatable(ls, className) == 0) {
		assert(false);
		return Reference {}; // name already in use
	}

	// Use the same table as a metatable and to store the class methods
	const int metatableIndex = lua_gettop(ls);

	// metatable.__index = metatable
	lua_pushvalue(ls, metatableIndex);
	lua_setfield(ls, metatableIndex, "__index");

	// Create table for uniqueness
	// lua_newtable(ls);
	/*lua_pushstring(ls, "v");
	lua_setfield(ls, -2, "__mode");
	lua_pushvalue(ls, -1);
	lua_setmetatable(ls, -2);*/
	// lua_setfenv(ls, -2);  // set table as env table

	/*
	// hide metatable from Lua getmetatable()
	lua_pushliteral(ls, "__metatable");
	lua_pushvalue(ls, methodsIndex);
	lua_settable(ls, metatableIndex);
*/

	if (baseClassID != nullTypeId) {
		// Get base table of methods
		const TypeName baseClassName = typeIdToName(baseClassID);
		assert(baseClassName);
		luaL_getmetatable(ls, baseClassName);
		assert(lua_istable(ls, -1));

		const int baseMetaTable = lua_gettop(ls);

		// metatable._base = base meta table
		// _base is used for type safety
		lua_pushvalue(ls, baseMetaTable);
		lua_setfield(ls, metatableIndex, "_base");

		// Create metatable for methods table
		lua_newtable(ls);
		lua_pushvalue(ls, -1); // dup metatable
		lua_setmetatable(ls, metatableIndex);
		const int mt = lua_gettop(ls);

		// methods.metatable.__index = baseMetaTable
		lua_pushliteral(ls, "__index");
		lua_pushvalue(ls, baseMetaTable);
		lua_rawset(ls, mt);

#if TY_LUABIND_TYPE_SAFE
		registerBaseClass(ls, classID, baseClassID);
#endif
	}

	// Register className as global so that Lua scripts can access it
	registerClassInGlobals(ls, className, metatableIndex);

	registerTypeName(classID, className);

	// Create reference to table
	lua_pushvalue(ls, metatableIndex);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

void registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, lua_CFunction destructor) {
	assert(closure);
	lua_pushcfunction(ls, closure);
	lua_setfield(ls, tableIndex, "new"); // table.new = new_T
	if (destructor) {
		lua_getmetatable(ls, tableIndex); // mt
		assert(lua_istable(ls, -1));
		lua_pushcfunction(ls, destructor); // mt, destructor
		lua_setfield(ls, -2, "__gc");      // mt.__gc = destructor
	}
}

void registerDeleteOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize) {
	lua_getmetatable(ls, tableIndex); // mt
	assert(lua_istable(ls, -1));
	pushFunctionAsUpvalue(ls, closure, &functionPtr, functionPtrSize); // mt, destructor
	lua_setfield(ls, -2, "__gc");      // mt.__gc = destructor
}

void registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize) {
	pushFunctionAsUpvalue(ls, closure, &functionPtr, functionPtrSize);
	lua_setfield(ls, tableIndex, "new"); // table.new = closure
}

int registerLuaClass(lua_State* ls) {
	const int nargs = lua_gettop(ls);
	if (nargs != 2) {
		return luaL_argerror(ls, 1, "wrong number of arguments");
	}
	// Arg: className, meta table
	if (! lua_isstring(ls, 1)) {
		return luaL_argerror(ls, 1, "bad argument");
	}
	if (! lua_istable(ls, 2)) {
		return luaL_argerror(ls, 2, "bad argument");
	}

	// Register meta table in registry.
	// registry[name] = metatable
	// This is used by scripts
	lua_pushvalue(ls, 1); // class name
	lua_pushvalue(ls, 2); // mt
	lua_settable(ls, LUA_REGISTRYINDEX);

	return 0;
}

int getClassMetatable(lua_State* ls) {
	if (! lua_isstring(ls, 1)) {
		return luaL_argerror(ls, 1, "bad argument");
	}
	const char* className = lua_tostring(ls, 1);
	lua_pushstring(ls, className);
	lua_rawget(ls, LUA_REGISTRYINDEX);
	return lua_istable(ls, -1) ? 1 : 0;
}

bool isAllocatedByLua(lua_State* ls, int userDataStackIndex) {
	bool res = false;
	if (! lua_isuserdata(ls, userDataStackIndex)) {
		return 0; // object registered as table
	}
	if (lua_getiuservalue(ls, userDataStackIndex, 1) != LUA_TNONE) {
		assert(lua_tointeger(ls, -1) == kLuaAllocated);
		res = true;
	}
	// else object was allocated by C++
	lua_pop(ls, 1); // pops value pushed by lua_getiuservalue
	return res;
}

} // namespace Typhoon::LuaBind::detail
