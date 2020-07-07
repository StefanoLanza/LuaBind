#include "class.h"
#include "autoBlock.h"
#include "detail.h"
#include "table.h"
#include <cassert>

struct undef;

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
	const int metatableIndex = lua_gettop(ls);

	// Create a new table of methods representing the class
	//lua_newtable(ls);
	const int methodsIndex = metatableIndex;// lua_gettop(ls);

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

	// metatable.__index = methodsIndex
	lua_pushliteral(ls, "__index");
	lua_pushvalue(ls, methodsIndex);
	lua_settable(ls, metatableIndex);

	if (baseClassID != nullTypeId) {
		// Get base table of methods
		const TypeName baseClassName = typeIdToName(baseClassID);
		assert(baseClassName);
		luaL_getmetatable(ls, baseClassName);
		if (lua_istable(ls, -1)) {
			const int baseMetaTable = lua_gettop(ls);
			lua_pushliteral(ls, "__index");
			lua_rawget(ls, baseMetaTable);
			assert(lua_istable(ls, -1));
			const int baseMethods = lua_gettop(ls);

			// metatable._base = base meta table
			// _base is used for type safety
			lua_pushliteral(ls, "_base");
			lua_pushvalue(ls, baseMetaTable);
			lua_settable(ls, metatableIndex);

			// Create metatable for methods table
			lua_newtable(ls);
			lua_pushvalue(ls, -1); // dup metatable
			lua_setmetatable(ls, methodsIndex);
			const int mt = lua_gettop(ls);

			// methods.metatable.__index = base.methods
			lua_pushliteral(ls, "__index");
			lua_pushvalue(ls, baseMethods);
			lua_rawset(ls, mt);
		}
		else {
			// base class has not been registered
			assert(false);
			return Reference {};
		}
	}

	// Register className as global so that Lua scripts can access it
	registerClassInGlobals(ls, className, methodsIndex);

	registerTypeName(classID, className);

	// Create reference to table
	lua_pushvalue(ls, methodsIndex);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

void registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure) {
	assert(closure);

	AutoBlock autoBlock(ls);

	// Create metatable for table of class methods
	if (! lua_getmetatable(ls, tableIndex)) {
		lua_newtable(ls);
		lua_pushvalue(ls, -1);
		lua_setmetatable(ls, tableIndex);
	}
	const int mt = lua_gettop(ls);

	// Set __call
	// Save className as upvalue
	lua_pushliteral(ls, "__call");
	lua_pushcfunction(ls, closure);
	lua_settable(ls, mt); // mt.__call = new_T
}

void registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize) {
	AutoBlock autoBlock(ls);

	// Create metatable for table of class methods
	if (! lua_getmetatable(ls, tableIndex)) {
		lua_newtable(ls);
		lua_pushvalue(ls, -1);
		lua_setmetatable(ls, tableIndex);
	}
	const int mt = lua_gettop(ls);

	lua_pushliteral(ls, "__call");
	// Push the closure with flags ==, so that we skip the first element on the stack (a table representing the caller)
	pushFunctionAsUpvalue(ls, closure, &functionPtr, functionPtrSize, detail::Flags::call);
	lua_settable(ls, mt); // mt.__call = closure
}

int registerLuaClass(lua_State* ls) {
	const int nargs = lua_gettop(ls);
	if (nargs != 2) {
		luaL_argerror(ls, 1, "wrong number of arguments");
	}
	// Arg: className, meta table
	if (! lua_isstring(ls, 1)) {
		luaL_argerror(ls, 1, "bad argument");
	}
	if (! lua_istable(ls, 2)) {
		luaL_argerror(ls, 2, "bad argument");
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
		luaL_argerror(ls, 1, "bad argument");
	}
	const char* className = lua_tostring(ls, 1);
	lua_pushstring(ls, className);
	lua_rawget(ls, LUA_REGISTRYINDEX);
	return lua_istable(ls, -1) ? 1 : 0;
}

} // namespace Typhoon::LuaBind::detail
