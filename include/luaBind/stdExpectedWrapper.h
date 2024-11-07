#pragma once

#ifdef __cpp_lib_expected

#include "typeWrapper.h"
#include <expected>

namespace Typhoon::LuaBind {

// Wrapper for std::expected
template <class T, class E>
class Wrapper<std::expected<T, E>> {
private:
	using ExpectedType = std::expected<T, E>;

public:
	static constexpr int stackSize = std::max(Wrapper<T>::stackSize, Wrapper<E>::stackSize);

	static int match(lua_State* ls, int idx) {
		return Wrapper<T>::match(ls, idx) || Wrapper<E>::match(ls, idx);
	}
	static void push(lua_State* ls, const ExpectedType& e) {
		if (e) {
			Wrapper<T>::push(ls, e.value());
		}
		else {
			Wrapper<E>::push(ls, e.error());
		}
	}
	static ExpectedType pop(lua_State* ls, int idx) {
		if (Wrapper<T>::match(ls, idx)) {
			return Wrapper<T>::pop(ls, idx);
		}
		else {
			return std::unexpected(Wrapper<E>::pop(ls, idx));
		}
	}
};

} // namespace Typhoon::LuaBind

#endif
