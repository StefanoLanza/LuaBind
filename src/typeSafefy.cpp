#include "typeSafefy.h"

#if TY_LUA_TYPE_SAFE

#include "autoBlock.h"
#include <unordered_map>

namespace Typhoon::LUA::detail {

namespace {

std::unordered_map<const void*, TypeId> pointerMap;

bool compatibleTypes(TypeId first, TypeId second) {
	// TODO compare base classes of first
	return first == second;
}
} // namespace

bool checkPointerType(const void* ptr, TypeId typeId) {
	auto it = pointerMap.find(ptr);
	return true; // FIXME (it != pointerMap.end()) && compatibleTypes(*it->second, typeId);
}

void registerPointerType(const void* ptr, TypeId typeId) {
	pointerMap.insert_or_assign(ptr, typeId);
}

void unregisterPointerType(const void* ptr) {
	pointerMap.erase(ptr);
}

bool checkType(lua_State* ls, int index, const char* className) {
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

} // namespace Typhoon::LUA::detail

#endif
