#include "typeSafefy.h"

#if TY_LUABIND_TYPE_SAFE

#include "autoBlock.h"
#include "context.h"

namespace Typhoon::LuaBind::detail {

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
	for (const auto& key : context->tempPointerMap) {
		if (key.first == ptr) {
			if (compatibleTypes(*context, key.second, typeId)) {
				return true;
			}
		}
	}
	return false;
}

void registerTemporaryPointer(lua_State* ls, const void* ptr, TypeId typeId) {
	auto context = getContext(ls);
	auto key = std::make_pair(ptr, typeId);
	context->tempPointerMap.push_back(key);
}

void* makePointerKey(const void* ptr, TypeId typeId) {
	// Last three bits of ptr are 0 since pointers are 8 bytes aligned
	// TypeId.impl does not have this constraint
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) | (reinterpret_cast<uintptr_t>(typeId.impl) & 0b111));
}

bool compatibleTypes(lua_State* ls, TypeId first, TypeId second) {
	auto context = getContext(ls);
	return compatibleTypes(*context, first, second);
}

} // namespace Typhoon::LuaBind::detail

#endif
