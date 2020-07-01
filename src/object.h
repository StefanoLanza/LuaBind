#pragma once

#include "autoBlock.h"
#include "result.h"
#include "table.h"
#include "stackUtils.h"
#include <cassert>

struct lua_State;

namespace Typhoon::LuaBind {

class Object {
public:
	Object(lua_State* ls, int ref)
	    : ls(ls)
	    , ref(ref) {
		assert(ref != LUA_NOREF);
	}

	bool hasMethod(const char* func) const;

	Result callMethod(const char* func) const;

	template <typename RetType>
	Result callMethodRet(const char* func, RetType& ret) const {
		AutoBlock autoBlock(ls);

		const auto [validCall, resStackIndex] = beginCall(func);
		if (! validCall) {
			return Result { false };
		}

		const int    nres = getStackSize<RetType>();
		const Result res = callMethodImpl(0, nres);
		if (res) {
			ret = get<RetType>(ls, resStackIndex);
		}
		return res;
	}

	template <typename... Args>
	Result callMethod(const char* func, Args&&... args) const {
		AutoBlock autoBlock(ls);

		const auto [validCall, resStackIndex] = beginCall(func);
		if (! validCall) {
			return Result { false };
		}

		// Push arguments
		// Call Push for each function argument
		// Because of C++ rules, by creating an array, Push is called in the correct order for each argument
		const int argStackSize[] = { push(ls, std::forward<Args>(args))..., 0 };

		// Get stack size of all arguments
		int narg = 0;
		for (size_t i = 0; i < sizeof...(Args); ++i) {
			narg += argStackSize[i];
		}

		return callMethodImpl(narg, 0);
	}

private:
	std::pair<bool, int> beginCall(const char* func) const;
	Result               callMethodImpl(int narg, int nres) const;

private:
	lua_State* ls;
	int        ref;
};

} // namespace Typhoon::LuaBind
