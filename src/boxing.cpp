#include "boxing.h"
#include <core/allocator.h>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind::detail {

extern Allocator* getAllocator(lua_State* ls);

void* allocateBoxed(lua_State* ls, size_t size, size_t alignment) {
	return getAllocator(ls)->alloc(size, alignment);
}

int collectBoxed(lua_State* ls) {
	const size_t size = static_cast<size_t>(lua_tonumber(ls, lua_upvalueindex(1)));
	// Extract pointer from user data
	void** const ptrptr = static_cast<void**>(lua_touserdata(ls, 1));
	getAllocator(ls)->free(*ptrptr, size);
	return 0;
}

} // namespace Typhoon::LuaBind::detail
