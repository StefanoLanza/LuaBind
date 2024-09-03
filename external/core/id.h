#pragma once

#include <cstdint>
#include <limits>

#if __cplusplus >= 202002L
#include <concepts>
#endif

namespace Typhoon {

// Strongly typed id
/* Example:
struct ComponentTag;  // no need to define it
using ComponentId = Id<ComponentTag, uint32_t>;
*/
template <typename Tag, typename Impl = uint32_t, Impl nullValue = 0>
#if __cplusplus >= 202002L
requires std::integral<Impl>
#endif
    struct Id {
	constexpr Id()
	    : value { nullValue } {
	}
	explicit Id(Impl value)
	    : value { value } {
	}
#if __cplusplus >= 202002L
	template <std::integral T>
	explicit Id(T value)
	    : value { static_cast<Impl>(value) } {
	}
#endif

	[[nodiscard]] explicit operator bool() const {
		return value != nullValue;
	}

	void set(Impl newValue) {
		value = newValue;
	}

	void reset() {
		value = nullValue;
	}

	[[nodiscard]] Impl getValue() const {
		return value;
	}

	[[nodiscard]] bool isValid() const {
		return value != nullValue;
	}

	[[nodiscard]] friend bool operator==(Id a, Id b) {
		return a.value == b.value;
	}
	[[nodiscard]] friend bool operator!=(Id a, Id b) {
		return a.value != b.value;
	}

	[[nodiscard]] constexpr static size_t maxIds() {
		return std::numeric_limits<Impl>::max() - 1;
	} // 1 id reserved for null

private:
	Impl value;
};

} // namespace Typhoon
