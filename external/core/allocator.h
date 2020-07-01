#pragma once

#include <core/uncopyable.h>

namespace Typhoon {

class Allocator : Uncopyable {
public:
	explicit Allocator(size_t defaultAlignment);
	virtual ~Allocator() = default;

	size_t GetDefaultAlignment() const {
		return m_defaultAlignment;
	}
	void*         Alloc(size_t bytes);
	virtual void* Alloc(size_t bytes, size_t alignment) = 0;
	virtual void  Free(void* ptr) = 0;
	void*         Realloc(void* ptr, size_t bytes);
	virtual void* Realloc(void* ptr, size_t bytes, size_t alignment) = 0;

	template <class T>
	T* Alloc(size_t size) {
		return reinterpret_cast<T*>(Alloc(size));
	}

	template <class T>
	T* Alloc() {
		return reinterpret_cast<T*>(Alloc(sizeof(T)));
	}

	template <class T>
	T* AllocArray(size_t numElements) {
		void* const memptr = Alloc(sizeof(T) * numElements);
		if (memptr) {
			// Construct elements
			for (size_t i = 0; i < numElements; ++i) {
				new (reinterpret_cast<char*>(memptr) + sizeof(T) * i) T;
			}
		}
		return reinterpret_cast<T*>(memptr);
	}

protected:
	size_t m_defaultAlignment;
};

inline void* Allocator::Alloc(size_t bytes) {
	return Alloc(bytes, m_defaultAlignment);
}

inline void* Allocator::Realloc(void* ptr, size_t bytes) {
	return Realloc(ptr, bytes, m_defaultAlignment);
}

template <class T>
inline void tdelete(T* p, Allocator* allocator) {
	if (p) {
		p->~T();
		allocator->Free(p);
	}
}

template <class T>
inline void tdelete_array(T* ptr, size_t numElements, Allocator* allocator) {
	if (ptr) {
		// Destruct elements
		for (size_t i = 0; i < numElements; ++i) {
			ptr[i].~T();
		}
		allocator->Free(ptr);
	}
}

} // namespace Typhoon

inline void* _cdecl operator new(size_t size, Typhoon::Allocator& allocator) throw() {
	return allocator.Alloc(size);
}

inline void _cdecl operator delete(void* ptr, Typhoon::Allocator& allocator) throw() {
	return allocator.Free(ptr);
}
