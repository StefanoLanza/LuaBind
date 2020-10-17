#include "boxing.h"
#include <core/allocator.h>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind::detail {

extern Allocator* boxedAllocator;

void* allocateBoxed(size_t size, size_t alignment) {
	return boxedAllocator->alloc(size, alignment);
}

int collectBoxed(lua_State* ls) {
	const size_t size = lua_tonumber(ls, lua_upvalueindex(1));
	// Extract pointer from user data
	void** const ptrptr = static_cast<void**>(lua_touserdata(ls, 1));
	boxedAllocator->free(*ptrptr, size);
	return 0;
}

} // namespace Typhoon::LuaBind::detail
