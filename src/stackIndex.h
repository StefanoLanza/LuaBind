#pragma once

namespace Typhoon::LUA {

//! Lua stack index wrapper
class StackIndex {
public:
	StackIndex()
	    : index(0) {
	}
	explicit constexpr StackIndex(int index)
	    : index(index) {
	}

	int GetIndex() const {
		return index;
	}
	bool IsValid() const {
		return (index != 0);
	}

private:
	int index;
};

// Index to top of the stack
constexpr StackIndex topStackIndex { -1 };
constexpr StackIndex invalidStackIndex { 0 };

inline bool operator==(StackIndex a, StackIndex b) {
	return (a.GetIndex() == b.GetIndex());
}

inline bool operator!=(StackIndex a, StackIndex b) {
	return ! (a == b);
}

} // namespace Typhoon::LUA
