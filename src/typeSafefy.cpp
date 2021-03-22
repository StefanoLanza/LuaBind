#include "typeSafefy.h"

#if TY_LUABIND_TYPE_SAFE

#include "autoBlock.h"
#include "context.h"
#include <unordered_map>

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
	Context* context = getContext(ls);
	context->baseClassMap.emplace(super, base);
}

bool tryCheckPointerType(lua_State* ls, const void* ptr, TypeId typeId) {
	Context* context = getContext(ls);
	auto it = context->pointerMap.find(ptr);
	return (it == context->pointerMap.end()) || compatibleTypes(*context, it->second, typeId);
}

bool checkPointerType(lua_State* ls, const void* ptr, TypeId typeId) {
	Context* context = getContext(ls);
	auto it = context->pointerMap.find(ptr);
	return (it != context->pointerMap.end()) && compatibleTypes(*context, it->second, typeId);
}

void registerPointer(lua_State* ls, const void* ptr, TypeId typeId) {
	Context* context = getContext(ls);
	context->pointerMap.insert_or_assign(ptr, typeId);
}

void unregisterPointer(lua_State* ls, const void* ptr) {
	Context* context = getContext(ls);
	context->pointerMap.erase(ptr);
}

} // namespace Typhoon::LuaBind::detail

#endif
