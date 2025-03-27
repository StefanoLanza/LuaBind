#include "luaBind.h"

#include <core/typeId.h>
#if TY_LUABIND_TYPE_SAFE
#include <unordered_map>
#endif

namespace Typhoon {
class ScopedAllocator;
}

namespace Typhoon::LuaBind {

struct Context {
	lua_State*       ls;
	Allocator*       allocator;
	LinearAllocator* tempAllocator;
	ScopedAllocator* scopedAllocator;
	ScopedAllocator* currScopedAllocator;
	MemoryStats      memoryStats;
	WarningFunction  warningFunction;

#if TY_LUABIND_TYPE_SAFE
	// FIXME Allocators
	std::unordered_map<TypeId, TypeId>          baseClassMap;
	std::vector<std::pair<const void*, TypeId>> tempPointerMap;
#endif
};

} // namespace Typhoon::LuaBind

namespace Typhoon::LuaBind::detail {

Context* getContext(lua_State* ls);

} // namespace Typhoon::LuaBind::detail
