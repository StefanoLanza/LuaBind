#include "object.h"
#include "autoBlock.h"
#include <cassert>

namespace Typhoon::LUA {

bool Object::hasMethod(const char* func) const {
	assert(func);
	AutoBlock autoBlock(ls);

	// Push self
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int stackIndex = lua_gettop(ls);
	assert(lua_isuserdata(ls, stackIndex) || lua_istable(ls, stackIndex));

	// Get function from the object's table
	lua_getfield(ls, stackIndex, func);
	return lua_isfunction(ls, -1);
}

Result Object::CallMethod(const char* func) const {
	AutoBlock autoBlock(ls);

	auto [validCall, resStackIndex] = BeginCall(func);
	if (! validCall) {
		return Result { false };
	}
	return CallMethodImpl(0, 0);
}

std::pair<bool, int> Object::BeginCall(const char* func) const {
	// Push self
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int stackIndex = lua_gettop(ls);

	assert(lua_isuserdata(ls, stackIndex) || lua_istable(ls, stackIndex));

	// Push method
	// Get method from the object's table
	lua_getfield(ls, stackIndex, func);
	if (! lua_isfunction(ls, -1)) {
		// Clean stack
		// FIXME One more pop?
		lua_pop(ls, 2);
		return { false, 0 };
	}
	// Push self
	lua_pushvalue(ls, stackIndex);

	// Return stack index where results are pushed
	return { true, stackIndex + 1 };
}

Result Object::CallMethodImpl(int narg, int nres) const {
	// Stack:
	// function
	// self
	// arg1
	// ..
	// argn     (-1)

	const int lres = lua_pcall(ls, 1 + narg, nres, 0);
	Result    res(true);
	if (0 != lres) {
		res = Result { lua_tostring(ls, -1) };
	}

	return res;
}

} // namespace Typhoon::LUA
