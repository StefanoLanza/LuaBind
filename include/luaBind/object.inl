#pragma once

#include "autoBlock.h"
#include "result.h"
#include "stackUtils.h"
#include <cassert>

namespace Typhoon::LuaBind {

template <typename RetType, typename... ArgTypes>
Result Object::callMethodRet(const char* func, RetType& ret, ArgTypes&&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return Result { false };
	}

	const int argStackSize[] = { getStackSize<ArgTypes>()..., 0 };

	// Push arguments
	// Because of C++ rules, by creating an array, push is called in the correct order for each argument
	const int dummy[] = { (push(ls, std::forward<ArgTypes>(args)), 1)..., 1 };

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	const int    nres = getStackSize<RetType>();
	const Result res = callMethodImpl(narg, nres);
	if (res) {
		ret = pop<RetType>(ls, resStackIndex);
	}
	return res;
}

template <typename... ArgTypes>
Result Object::callMethod(const char* func, ArgTypes&&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return Result { false };
	}

	const int argStackSize[] = { getStackSize<ArgTypes>()..., 0 };

	// Push arguments
	// Because of C++ rules, by creating an array, push is called in the correct order for each argument
	const int dummy[] = { (push(ls, std::forward<ArgTypes>(args)), 1)..., 1 };

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	return callMethodImpl(narg, 0);
}

} // namespace Typhoon::LuaBind
