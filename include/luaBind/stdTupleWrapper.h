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

	template <std::size_t... argIndices>
	static TupleType popAll(lua_State* ls, int idx, std::index_sequence<argIndices...> /*indx*/) {
		// Get stack size of all arguments
		constexpr int argStackSize[] = { Wrapper<Args>::stackSize..., 0 };

		// Compute stack indices
		int argStackIndex[sizeof...(Args) + 1] = {};
		argStackIndex[0] = idx;
		for (size_t i = 1; i < sizeof...(Args); ++i) {
			argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
		}
		return { Wrapper<Args>::pop(ls, argStackIndex[argIndices])... };
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
		return popAll(ls, idx, std::index_sequence_for<Args...> {});
	}
};

} // namespace Typhoon::LuaBind
