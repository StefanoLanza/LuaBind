#include "typeSafefy.h"

#if TY_LUABIND_TYPE_SAFE

#include "autoBlock.h"
#include <unordered_map>

namespace Typhoon::LuaBind::detail {

	std::unordered_map<TypeId, TypeId> baseClassMap;//FIXME in a context, allocator

namespace {

std::unordered_map<const void*, TypeId> pointerMap;//FIXME in a context, allocator

bool compatibleTypes(TypeId first, TypeId second) {
	while (first != second) {
		// Query base class
		auto it = baseClassMap.find(first);
		if (it == baseClassMap.end()) {
			return false;
		}
		first = it->second;
	}
	return true;
}

} // namespace

bool tryCheckPointerType(const void* ptr, TypeId typeId) {
	auto it = pointerMap.find(ptr);
	return (it == pointerMap.end()) || compatibleTypes(it->second, typeId);
}

bool checkPointerType(const void* ptr, TypeId typeId) {
	auto it = pointerMap.find(ptr);
	return (it != pointerMap.end()) && compatibleTypes(it->second, typeId);
}

void registerPointerWithType(const void* ptr, TypeId typeId) {
	pointerMap.insert_or_assign(ptr, typeId);
}

void unregisterPointer(const void* ptr) {
	pointerMap.erase(ptr);
}

} // namespace Typhoon::LuaBind::detail

#endif
