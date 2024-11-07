#pragma once

#include "typeWrapper.h"
#include <utility>

namespace Typhoon::LuaBind {

// Wrapper for std::pair
template <typename T1, typename T2>
class Wrapper<std::pair<T1, T2>> {
	using pairType = std::pair<T1, T2>;
public:
	static constexpr int stackSize = Wrapper<T1>::stackSize + Wrapper<T2>::stackSize;

	static int match(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return Wrapper<T1>::match(ls, idx) && Wrapper<T2>::match(ls, idx2);
	}
	static void push(lua_State* ls, const pairType& pair) {
		Wrapper<T1>::push(ls, pair.first);
		Wrapper<T2>::push(ls, pair.second);
	}
	static pairType pop(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return { Wrapper<T1>::pop(ls, idx), Wrapper<T2>::pop(ls, idx2) };
	}
};

} // namespace Typhoon::LuaBind
