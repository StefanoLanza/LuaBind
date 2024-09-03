#pragma once

#include "autoBlock.h"
#include "result.h"
#include "stackUtils.h"
#include <cassert>

namespace Typhoon::LuaBind {

template <typename RetType, typename... ArgTypes>
Result Object::callMethodRet(const char* func, RetType& ret, const ArgTypes&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return Result { false };
	}

	constexpr int argStackSize[] = { Wrapper<const ArgTypes&>::stackSize..., 0 };

	// Push arguments
	// Because of C++ rules, by creating an array, push is called in the correct order for each argument
	const int dummy[] = { (Wrapper<const ArgTypes&>::push(ls, args), 1)..., 1 };
	(void)dummy;

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	constexpr int nres = Wrapper<RetType>::stackSize;
	const Result  res = callMethodImpl(narg, nres);
	if (res) {
		ret = Wrapper<RetType>::pop(ls, resStackIndex);
	}
	return res;
}

template <typename... ArgTypes>
Result Object::callMethod(const char* func, const ArgTypes&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return Result { false };
	}

	constexpr int argStackSize[] = { Wrapper<const ArgTypes&>::stackSize..., 0 };

	// Push arguments
	// Because of C++ rules, by creating an array, push is called in the correct order for each argument
	const int dummy[] = { (Wrapper<const ArgTypes&>::push(ls, args), 1)..., 1 };

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	return callMethodImpl(narg, 0);
}

} // namespace Typhoon::LuaBind
