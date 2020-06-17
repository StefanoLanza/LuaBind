#pragma once

#include <utility>

struct lua_State;

namespace Typhoon::LUA {

// Wrapper for std::pair
template <typename T1, typename T2>
class Wrapper<std::pair<T1, T2>> {
public:
	using pairType = std::pair<T1, T2>;
	static int Match(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return Wrapper<T1>::Match(ls, idx) && Wrapper<T2>::Match(ls, idx2);
	}
	static int Push(lua_State* ls, const pairType& pair) {
		int n = 0;
		n += LUA::Push(ls, pair.first);
		n += LUA::Push(ls, pair.second);
		return n;
	};
	static pairType Get(lua_State* ls, int idx) {
		const int idx2 = idx + Wrapper<T1>::stackSize;
		return { Wrapper<T1>::Get(ls, idx), Wrapper<T2>::Get(ls, idx2) };
	}
	static constexpr int stackSize = Wrapper<T1>::stackSize + Wrapper<T2>::stackSize;
};

} // namespace Typhoon::LUA
