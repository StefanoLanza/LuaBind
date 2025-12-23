#pragma once

#include <core/allocator.h>
#include <vector>

namespace Typhoon {

template <typename T>
class stdAllocator {
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	constexpr explicit stdAllocator(Allocator& allocator) noexcept
	    : allocator { allocator } {
	}

	~stdAllocator() = default;
	constexpr stdAllocator(const stdAllocator& rhs) noexcept = default;
	template <typename U>
	constexpr stdAllocator(const stdAllocator<U>& other)
	    : allocator(other.allocator) {
	}
	stdAllocator& operator=(const stdAllocator&) = delete;

	[[nodiscard]] constexpr T* allocate(std::size_t n) {
		return static_cast<T*>(allocator.alloc(n * sizeof(T), alignof(T)));
	}

	constexpr void deallocate(T* p, std::size_t n) {
		allocator.free(p, n * sizeof(T));
	}

	bool operator==(const stdAllocator& rhs) {
		return &allocator == &rhs.allocator;
	}

	bool operator!=(const stdAllocator& rhs) {
		return ! ((*this) == rhs);
	}

	template <class U>
	friend class stdAllocator;

private:
	Allocator& allocator;
};

template <typename T1, typename T2>
bool operator==(const stdAllocator<T1>& a1, const stdAllocator<T2>& a2) {
	return &a1.allocator == &a2.allocator;
}

template <typename T1, typename T2>
bool operator!=(const stdAllocator<T1>& a1, const stdAllocator<T2>& a2) {
	return ! (a1 == a2);
}

template <class T>
using stdVector = std::vector<T, stdAllocator<T>>;

} // namespace Typhoon
