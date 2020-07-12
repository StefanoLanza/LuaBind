#include "typeWrapper.h"
#include <cassert>
#include <core/ptrUtil.h>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

template <class T>
int box(lua_State* ls) {
	void* mem = allocateBoxed(sizeof(T), std::alignment_of_v<T>);
	T*    boxed = new (mem) T;

	// Optionally initialize the boxed object
	if (const T* value = static_cast<const T*>(lua_touserdata(ls, 1)); value) {
		*boxed = *value;
	}

	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof boxed);
	// Construct new object and store a pointer to its pointer in the user data
	std::memcpy(ud, &boxed, sizeof boxed);

	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                    // ud, mt
	lua_pushvalue(ls, -1);               // ud, mt, mt
	lua_pushcfunction(ls, collectBoxed); // ud, mt, mt, collectBoxed
	lua_setfield(ls, -2, "__gc");        // ud, mt, mt
	lua_setmetatable(ls, -3);            // ud, mt

	lua_pop(ls, 1); // ud

	return 1;
}

template <class T>
int store(lua_State* ls) {
	// Stack: boxed (ud), value(ud)
	assert(lua_isuserdata(ls, 1));
	assert(lua_isuserdata(ls, 2));
	// Extract pointer from userdata
	T* boxed = serializePOD<T*>(lua_touserdata(ls, 1));
	// Store value in boxed object
	*boxed = Wrapper<T>::Get(ls, 2);
	return 0;
}

template <class T>
int retrieve(lua_State* ls) {
	// Stack: boxed (ud)
	if (! lua_isuserdata(ls, 1)) {
		return 0;
	}
	// Extract pointer from userdata
	const T* boxed = serializePOD<T*>(lua_touserdata(ls, 1));
	// Push boxed object
	return Wrapper<T>::Push(ls, *boxed);
}

template <class T>
void pushBoxingFunctions(lua_State* ls, int tableStackIndex) {
	static_assert(std::is_trivially_destructible_v<T>, "Type must be trivially destructible");

	lua_pushliteral(ls, "box");
	lua_pushcfunction(ls, box<T>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "store");
	lua_pushcfunction(ls, store<T>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "retrieve");
	lua_pushcfunction(ls, retrieve<T>);
	lua_settable(ls, tableStackIndex);
}
} // namespace Typhoon::LuaBind::detail
