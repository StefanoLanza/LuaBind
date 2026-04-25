#pragma once

#include "autoBlock.h"
#include "result.h"
#include "stackUtils.h"
#include <cassert>

namespace Typhoon::LuaBind {

template <typename RetType, typename... ArgTypes>
ResultT<RetType> Object::callMethodRet(const char* func, const ArgTypes&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return UNEXPECTED("Invalid call");
	}

	constexpr int argStackSize[] = { Wrapper<const ArgTypes&>::getStackSize()..., 0 };

	// Push arguments
	// Fold expression (C++17 and later). The comma operator ensures evaluation from left to right
	(Wrapper<const ArgTypes&>::push(ls, args), ...);

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	constexpr int nres = Wrapper<RetType>::getStackSize();
	if (const auto res = callMethodImpl(narg, nres); ! res) {
		return UNEXPECTED(res.error());
	}

	return Wrapper<RetType>::pop(ls, resStackIndex);
}

template <typename... ArgTypes>
Result Object::callMethod(const char* func, const ArgTypes&... args) const {
	AutoBlock autoBlock(ls);

	const auto [validCall, resStackIndex] = beginCall(func);
	if (! validCall) {
		return UNEXPECTED("invalid call");
	}

	constexpr int argStackSize[] = { Wrapper<const ArgTypes&>::getStackSize()..., 0 };

	// Push arguments
	// Fold expression (C++17 and later). The comma operator ensures evaluation from left to right
	(Wrapper<const ArgTypes&>::push(ls, args), ...);

	// Get stack size of all arguments
	int narg = 0;
	for (size_t i = 0; i < sizeof...(ArgTypes); ++i) {
		narg += argStackSize[i];
	}

	return callMethodImpl(narg, 0);
}

} // namespace Typhoon::LuaBind
