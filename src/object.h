#pragma once

#include "autoBlock.h"
#include "table.h"
#include "typeWrapper.h"
#include <cassert>
#include <core/result.h>
#include <string>

struct lua_State;

namespace Typhoon::LUA {

class Object {
public:
	Object(lua_State* ls, int ref)
	    : ls(ls)
	    , ref(ref) {
		assert(ref != LUA_NOREF);
	}

	bool hasMethod(const char* func) const;

	Result CallMethod(const char* func) const;

	template <typename RetType>
	Result CallMethodRet(const char* func, RetType& ret) const {
		AutoBlock autoBlock(ls);

		const auto [validCall, resStackIndex] = BeginCall(func);
		if (! validCall) {
			return Result { false };
		}

		const int    nres = Wrapper<RetType>::stackSize;
		const Result res = CallMethodImpl(0, nres);
		if (res) {
			ret = Wrapper<RetType>::Get(ls, resStackIndex);
		}
		return res;
	}

	template <typename... Args>
	Result CallMethod(const char* func, Args&&... args) const {
		AutoBlock autoBlock(ls);

		const auto [validCall, resStackIndex] = BeginCall(func);
		if (! validCall) {
			return Result { false };
		}

		// Push arguments
		// Call Push for each function argument
		// Because of C++ rules, by creating an array, Push is called in the correct order for each argument
		const int argStackSize[] = { Push(ls, std::forward<Args>(args))..., 0 };

		// Get stack size of all arguments
		int narg = 0;
		for (size_t i = 0; i < sizeof...(Args); ++i) {
			narg += argStackSize[i];
		}

		return CallMethodImpl(narg, 0);
	}

private:
	std::pair<bool, int> BeginCall(const char* func) const;
	Result               CallMethodImpl(int narg, int nres) const;

private:
	lua_State* ls;
	int        ref;
};

} // namespace Typhoon::LUA
