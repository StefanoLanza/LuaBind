#pragma once

#include <lua/src/lua.hpp>

namespace Typhoon::LUA {

// Lua reference wrapper
class Reference {
public:
	constexpr Reference()
	    : value(LUA_NOREF) {
	}
	explicit Reference(int value)
	    : value(value) {
	}

	int GetValue() const {
		return value;
	}
	bool IsValid() const {
		return (value != LUA_NOREF);
	}
	void Release() {
		value = LUA_NOREF;
	}
	explicit operator bool() const {
		return IsValid();
	}

private:
	int value;
};

constexpr Reference nullRef;

inline bool operator==(Reference a, Reference b) {
	return (a.GetValue() == b.GetValue());
}

inline bool operator!=(Reference a, Reference b) {
	return ! (a == b);
}

} // namespace Typhoon::LUA
