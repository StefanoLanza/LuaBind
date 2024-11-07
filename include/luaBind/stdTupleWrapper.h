#pragma once

#include "typeWrapper.h"
#include <tuple>

namespace Typhoon::LuaBind {

// Wrapper for std::pair
template <typename... Args>
class Wrapper<std::tuple<Args...>> {
private:
	using TupleType = std::tuple<Args...>;

	template <size_t index>
	static constexpr int getTotalStackSize() {
		using E = std::tuple_element_t<index, TupleType>;
		int stackSize = Wrapper<E>::stackSize;
		if constexpr (index + 1 < std::tuple_size_v<TupleType>) {
			stackSize += getTotalStackSize<index + 1>();
		}
		return stackSize;
	}

	template <size_t index>
	static bool getMatch(lua_State* ls, int idx) {
		using E = std::tuple_element_t<index, TupleType>;
		bool match = Wrapper<E>::match(ls, idx);
		if constexpr (index + 1 < std::tuple_size_v<TupleType>) {
			match = match && getMatch<index + 1>(ls, Wrapper<E>::stackSize + idx);
		}
		return match;
	}

	template <size_t index>
	static void popAll(TupleType& tuple, lua_State* ls, int idx) {
		using E = std::tuple_element_t<index, TupleType>;
		std::get<index>(tuple) = Wrapper<E>::pop(ls, idx);
		if constexpr (index + 1 < std::tuple_size_v<TupleType>) {
			popAll<index + 1>(tuple, ls, Wrapper<E>::stackSize + idx);
		}
	}

public:
	static constexpr int stackSize = getTotalStackSize<0>();

	static int match(lua_State* ls, int idx) {
		return getMatch<0>(ls, idx);
	}
	static void push(lua_State* ls, const TupleType& tuple) {
		std::apply([ls](auto&... x) { (..., LuaBind::push(ls, x)); }, tuple);
	}
	static TupleType pop(lua_State* ls, int idx) {
		TupleType t {};
		popAll<0>(t, ls, idx);
		return t;
	}

private:
};

} // namespace Typhoon::LuaBind
