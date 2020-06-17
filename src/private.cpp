#include "private.h"
#include "autoBlock.h"
#include "class.h"
#include "table.h"
#include <cassert>
#include <core/allocator.h>
#include <core/linearAllocator.h>

namespace Typhoon::LUA {

extern LinearAllocator* temporaryAllocator;
extern Allocator*       boxedAllocator;

namespace detail {
void* AllocateBoxed(size_t size, size_t alignment) {
	return boxedAllocator->Alloc(size, alignment);
}

void* allocTemporary(size_t size, size_t alignment) {
	void* ptr = temporaryAllocator->Allocate(size, alignment);
	assert(ptr != nullptr);
	return ptr;
}

bool CheckType(lua_State* ls, int index, const char* className) {
	assert(index >= 1);
	AutoBlock autoBlock(ls);
	lua_getfield(ls, LUA_REGISTRYINDEX, className);
	lua_getmetatable(ls, index);
	// Stack:
	// -2: table containing the class methods
	// -1: userdata's meta table
	while (lua_istable(ls, -1)) {
		if (lua_rawequal(ls, -1, -2)) {
			return true;
		}
		lua_getfield(ls, -1, "_base");
		lua_replace(ls, -2);
	}
	return false;
}

void* retrievePointerFromTable(lua_State* ls, int idx) {
	void* ptr = nullptr;
	if (lua_istable(ls, idx)) {
		lua_getfield(ls, idx, "_ptr");
		if (! lua_isuserdata(ls, -1)) {
			lua_pop(ls, 1);
			luaL_error(ls, "cannot retrieve raw pointer to C++ object: invalid _ptr field");
			return nullptr;
		}
		ptr = lua_touserdata(ls, -1);
		lua_pop(ls, 1);
	}
	else {
		luaL_error(ls, "argument is not a table");
	}
	return ptr;
}

void PushObjectAsFullUserData(lua_State* ls, void* objectPtr, const char* className) {
	assert(objectPtr);
	assert(className);

	void* const userData = lua_newuserdata(ls, sizeof objectPtr);
	// Copy C++ pointer to Lua userdata
	std::memcpy(userData, &objectPtr, sizeof objectPtr);

	// Lookup class metatable in registry
	luaL_getmetatable(ls, className);
	assert(lua_istable(ls, -1));

	// Set metatable of user data
	lua_setmetatable(ls, -2);
}

int CollectBoxed(lua_State* ls) {
	// Extract pointer from user data
	void** const ptrptr = static_cast<void**>(lua_touserdata(ls, 1));
	boxedAllocator->Free(*ptrptr);
	return 0;
}

void PushFunctionAsUpvalue(lua_State* ls, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize, Flags flags) {
	// Save pointer to caller and function as upvalue
	uint8_t* buffer = static_cast<uint8_t*>(lua_newuserdata(ls, functionPtrSize + sizeof(flags)));
	std::memcpy(buffer, functionPtr, functionPtrSize);
	std::memcpy(buffer + functionPtrSize, &flags, sizeof flags);
	lua_pushcclosure(ls, closure, 1);
}



} // namespace detail

} // namespace Typhoon::LUA
