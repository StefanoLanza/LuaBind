#pragma once

#include "allocator.h"
#include <type_traits>

namespace Typhoon {

class ScopedAllocator {
public:
	explicit ScopedAllocator(LinearAllocator& allocator);
	~ScopedAllocator();

	template <class T, class... ArgTypes>
	T* make(ArgTypes&&... args) {
		Destructor destructor = nullptr;
		if constexpr (! std::is_trivially_destructible_v<T>) {
			destructor = destructorCall<T>;
		}
		T* ptr = allocator.construct<T>(std::forward<ArgTypes>(args)...);
		registerObject(ptr, sizeof(T), destructor);
		return ptr;
	}

	template <class T>
	T* allocArray(size_t elementCount) {
		T* ptr = allocator.allocArray<T>(elementCount);
		if constexpr (std::is_trivially_destructible_v<T>) {
			// Register first element only, to rewind the allocator
			registerObject(ptr, sizeof(T), nullptr);
		}
		else {
			// Register all array elements, from last to first
			for (size_t i = elementCount; i > 0; --i) {
				registerObject(ptr + i - 1, sizeof(T), destructorCall<T>);
			}
		}
		return ptr;
	}

	void destroyAll();

private:
	using Destructor = void (*)(void* ptr);
	void registerObject(void* obj, size_t objSize, Destructor destructor);

	template <typename T>
	static void destructorCall(void* ptr) {
		static_cast<T*>(ptr)->~T();
	}

private:
	struct Finalizer;
	LinearAllocator& allocator;
	Finalizer*       finalizerHead;
};

} // namespace Typhoon
