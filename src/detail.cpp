#include "detail.h"
#include "autoBlock.h"
#include "class.h"
#include "table.h"
#include "context.h"
#include <cassert>
#include <core/hash.h>
#include <memory>

namespace Typhoon::LuaBind::detail {

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

void pushFunctionAsUpvalue(lua_State* ls, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize) {
	// Save pointer to caller and function as upvalue
	void* buffer = lua_newuserdatauv(ls, functionPtrSize, 0);
	std::memcpy(buffer, functionPtr, functionPtrSize);
	lua_pushcclosure(ls, closure, 1);
}

/*
Pointers can alias
e.g.
class A;
class B {
    A a;
};
B b;
&b == &b.a;
As they are cached in the registry, we need a key that also depends on their actual type
*/
lua_Integer makePointerKey(const void* ptr, TypeId typeId) {
	return hash64(std::make_pair(ptr, typeId));
}

ScopedAllocator* getTemporaryAllocator(lua_State* ls) {
	return getContext(ls)->currScopedAllocator;
}

Allocator* getAllocator(lua_State* ls) {
	return getContext(ls)->allocator;
}

#if TY_LUABIND_TYPE_SAFE

namespace {

bool compatibleTypes(const Context& context, TypeId first, TypeId second) {
	while (first != second) {
		// Query base class
		auto it = context.baseClassMap.find(first);
		if (it == context.baseClassMap.end()) {
			return false;
		}
		first = it->second;
	}
	return true;
}

} // namespace

void registerBaseClass(lua_State* ls, TypeId super, TypeId base) {
	auto context = getContext(ls);
	context->baseClassMap.emplace(super, base);
}

bool checkPointerType(lua_State* ls, const void* ptr, TypeId typeId) {
	auto context = getContext(ls);
	bool checked = false;
	for (const auto& key : context->tempPointerMap) {
		if (key.first == ptr) {
			checked = true;
			if (compatibleTypes(*context, key.second, typeId)) {
				return true;
			}
		}
	}
	return ! checked;
}

void registerTemporaryPointer(lua_State* ls, const void* ptr, TypeId typeId) {
	auto context = getContext(ls);
	auto key = std::make_pair(ptr, typeId);
	context->tempPointerMap.push_back(key);
}

bool compatibleTypes(lua_State* ls, TypeId first, TypeId second) {
	auto context = getContext(ls);
	return compatibleTypes(*context, first, second);
}

#endif

} // namespace Typhoon::LuaBind::detail
