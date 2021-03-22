#include "luaBind.h"

#include <core/allocator.h>
#include <core/typeId.h>
#if TY_LUABIND_TYPE_SAFETY
#include <unordered_map>
#endif

namespace Typhoon::LuaBind {

struct Context {
	lua_State* ls;
	Allocator* allocator;
	LinearAllocator* tempAllocator;
	MemoryStats memoryStats;

#if TY_LUABIND_TYPE_SAFE
	// FIXME Allocators
	std::unordered_map<TypeId, TypeId> baseClassMap;
	std::unordered_map<const void*, TypeId> pointerMap;
#endif
};

}

namespace Typhoon::LuaBind::detail {

Context* getContext(lua_State* ls);

} // namespace Typhoon::LuaBind::detail
