#pragma once

#include "typeWrapper.h"
#include <utility>

namespace Typhoon::LuaBind {

// Wrapper for std::pair
template <typename T1, typename T2>
class Wrapper<std::pair<T1, T2>> {
	using pairType = std::pair<T1, T2>;
public:
	static int match(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return Wrapper<T1>::Match(ls, idx) && Wrapper<T2>::Match(ls, idx2);
	}
	static int push(lua_State* ls, const pairType& pair) {
		int n = 0;
		n += Wrapper<T1>::push(ls, pair.first);
		n += Wrapper<T2>::push(ls, pair.second);
		return n;
	};
	static pairType pop(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return { Wrapper<T1>::pop(ls, idx), Wrapper<T2>::pop(ls, idx2) };
	}
	static constexpr int stackSize = Wrapper<T1>::stackSize + Wrapper<T2>::stackSize;
};

} // namespace Typhoon::LuaBind
