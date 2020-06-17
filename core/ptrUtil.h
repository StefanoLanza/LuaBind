#pragma once

#include <cstddef>

namespace Typhoon {

inline uintptr_t alignPointer(uintptr_t value, size_t alignment) {
	return (value + (alignment - 1)) & (~(alignment - 1));
}

inline void* alignPointer(void* ptr, size_t alignment) {
	return reinterpret_cast<void*>(alignPointer(reinterpret_cast<uintptr_t>(ptr), alignment));
}

inline size_t alignSize(uintptr_t size, size_t alignment) {
	return (size + (alignment - 1)) & (~(alignment - 1));
}

inline void* advancePointer(void* ptr, ptrdiff_t offset) {
	return static_cast<std::byte*>(ptr) + offset;
}

inline const void* advancePointer(const void* ptr, ptrdiff_t offset) {
	return static_cast<const std::byte*>(ptr) + offset;
}

template <typename T>
inline T* advancePointer(T* ptr, ptrdiff_t offset) {
	return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
}

} // namespace Typhoon
