#include "typeSafefy.h"

#if TY_LUA_TYPE_SAFE

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

} // namespace Typhoon::LUA::detail

#endif
