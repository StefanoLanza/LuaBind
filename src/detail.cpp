#include "detail.h"
#include "autoBlock.h"
#include "class.h"
#include "table.h"
#include <cassert>
#include <core/scopedAllocator.h>
#include <core/hash.h>
#include <memory>

namespace Typhoon::LuaBind::detail {

void* retrievePointerFromTable(lua_State* ls, int idx) {
	void* ptr = nullptr;
	if (lua_istable(ls, idx)) {
		lua_getfield(ls, idx, "_ptr");
		if (! lua_isuserdata(ls, -1)) {
			lua_pop(ls, 1);
			luaL_error(ls, "cannot retrieve raw pointer to C++ object: invalid _ptr field");
			return nullptr;
		}
		ptr = lua_touserdata(ls, -1);
		lua_pop(ls, 1);
	}
	else {
		luaL_error(ls, "argument is not a table");
	}
	return ptr;
}

void pushFunctionAsUpvalue(lua_State* ls, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize) {
	// Save pointer to caller and function as upvalue
	void* buffer = lua_newuserdatauv(ls, functionPtrSize, 0);
	std::memcpy(buffer, functionPtr, functionPtrSize);
	lua_pushcclosure(ls, closure, 1);
}

/* 
Pointers can alias
e.g.
class A;
class B {
	A a;
};
B b;
&b == &b.a;
As they are cached in the registry, we need a key that also depends on their actual type
*/
lua_Integer makePointerKey(const void* ptr, TypeId typeId) {
	return hash64(std::make_pair(ptr, typeId));
}

} // namespace Typhoon::LuaBind::detail
