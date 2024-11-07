#pragma once

#ifdef __cpp_lib_optional

#include "typeWrapper.h"
#include <optional>

namespace Typhoon::LuaBind {

// Wrapper for std::optional
template <class T>
class Wrapper<std::optional<T>> {
private:
	using OptionalType = std::optional<T>;

public:
	static constexpr int stackSize = std::max(Wrapper<T>::stackSize, Wrapper<Nil>::stackSize);

	static int match(lua_State* ls, int idx) {
		return Wrapper<T>::match(ls, idx) || Wrapper<Nil>::match(ls, idx);
	}
	static void push(lua_State* ls, const OptionalType& e) {
		if (e) {
			Wrapper<T>::push(ls, e.value());
		}
		else {
			lua_pushnil(ls);
		}
	}
	static OptionalType pop(lua_State* ls, int idx) {
		if (Wrapper<T>::match(ls, idx)) {
			return Wrapper<T>::pop(ls, idx);
		}
		else {
			return std::nullopt;
		}
	}
};

} // namespace Typhoon::LuaBind

#endif
