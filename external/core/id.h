#pragma once

#include <cstdint>

namespace Typhoon {

// Strongly typed id
/* Example:
struct ComponentTag;  // no need to define it
using ComponentId = Id<ComponentTag, uint32_t>;
*/
template <class Tag, class Impl = uint32_t, Impl nullValue = 0>
struct Id {
	constexpr Id()
	    : value { nullValue } {
	}
	explicit Id(Impl value)
	    : value { value } {
	}

	explicit operator bool() const {
		return value != nullValue;
	}

	void reset() {
		value = nullValue;
	}
	
	Impl getValue() const {
		return value;
	}

	friend bool operator==(Id a, Id b) {
		return a.value == b.value;
	}
	friend bool operator!=(Id a, Id b) {
		return a.value != b.value;
	}

private:
	Impl value;
};

} // namespace Typhoon
