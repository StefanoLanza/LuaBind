#pragma once

#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind {

// Lua reference wrapper
class Reference {
public:
	constexpr Reference()
	    : value(LUA_NOREF) {
	}
	explicit Reference(int value)
	    : value(value) {
	}

	int getValue() const {
		return value;
	}
	bool isValid() const {
		return (value != LUA_NOREF);
	}
	void Release() {
		value = LUA_NOREF;
	}
	explicit operator bool() const {
		return isValid();
	}

private:
	int value;
};

constexpr Reference nullRef;

inline bool operator==(Reference a, Reference b) {
	return (a.getValue() == b.getValue());
}

inline bool operator!=(Reference a, Reference b) {
	return ! (a == b);
}

} // namespace Typhoon::LuaBind
