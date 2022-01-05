#pragma once

#include <core/allocator.h>

namespace Typhoon {

class Allocator;

template <typename T>
class stdAllocator {
public:
	typedef T*       pointer;
	typedef const T* const_pointer;
	typedef T&       reference;
	typedef const T& const_reference;
	typedef T        value_type;

	template <typename U>
	struct rebind {
		typedef stdAllocator<U> other;
	};

	explicit stdAllocator(Allocator& allocator)
	    : allocator(allocator) {
	}

	~stdAllocator() = default;
	stdAllocator(const stdAllocator& rhs) = default;
	template <typename U>
	stdAllocator(const stdAllocator<U>& rhs)
	    : allocator(rhs.allocator) {
	}
	stdAllocator& operator=(const stdAllocator&) = delete;

	pointer allocate(size_t size) {
		return static_cast<pointer>(allocator.alloc(size * sizeof(T), alignof(T)));
	}

	void deallocate(pointer p, size_t count) {
		allocator.free(p, count * sizeof(T));
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

} // namespace Typhoon
