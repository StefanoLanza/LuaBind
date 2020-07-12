#include "boxing.h"
#include <core/allocator.h>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind::detail {

extern Allocator* boxedAllocator;

void* allocateBoxed(size_t size, size_t alignment) {
	return boxedAllocator->Alloc(size, alignment);
}

int collectBoxed(lua_State* ls) {
	// Extract pointer from user data
	void** const ptrptr = static_cast<void**>(lua_touserdata(ls, 1));
	boxedAllocator->Free(*ptrptr);
	return 0;
}

} // namespace Typhoon::LuaBind::detail
