#pragma once

#include "allocator.h"
#include <type_traits>

namespace Typhoon {

template <typename T>
void destructorCall(void* ptr) {
	static_cast<T*>(ptr)->~T();
}

using Destructor = void (*)(void* ptr);

class ScopedAllocator {
public:
	ScopedAllocator(Allocator& allocator);
	~ScopedAllocator();

	template <class T, class... ArgTypes>
	T* make(ArgTypes... args) {
		Destructor destructor = nullptr;
		if constexpr (! std::is_trivially_default_constructible_v<T>) {
			destructor = destructorCall<T>;
		}
		void* ptr = allocator.alloc(sizeof(T), alignof(T));
		registerObject(ptr, sizeof(T), destructor);
		return new (ptr) T(std::forward<ArgTypes>(args)...);
	}

	void* alloc(size_t size, size_t alignment);

private:
	void registerObject(void* obj, size_t objSize, Destructor destructor);

private:
	struct Finalizer;
	Allocator& allocator;
	Finalizer* finalizerHead;
};

} // namespace Typhoon
