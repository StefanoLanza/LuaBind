#pragma once

#include <cstdint>

namespace Typhoon {

// Strongly typed id
/* Example:
struct ComponentTag;  // no need to define it
using ComponentId = Id<ComponentTag, uint32_t>;
*/
template <class Tag, class Impl = uint32_t>
struct Id {
	constexpr Id()
	    : value(0) {
	}
	explicit Id(Impl value)
	    : value(value) {
	}

	explicit operator bool() const {
		return value != 0;
	}
	void reset() {
		value = 0;
	}
	Impl getValue() const {
		return value;
	}

	Impl value;

	friend bool operator==(Id a, Id b) {
		return a.value == b.value;
	}
	friend bool operator!=(Id a, Id b) {
		return a.value != b.value;
	}
};

} // namespace Typhoon
