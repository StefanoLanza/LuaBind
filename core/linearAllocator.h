#pragma once

#include <core/uncopyable.h>
#include <type_traits>

namespace Typhoon {

class Allocator;

class LinearAllocator : Uncopyable {
public:
	explicit LinearAllocator(Allocator& allocator, size_t capacity);
	explicit LinearAllocator(size_t capacity, size_t alignment = alignof(max_align_t));
	~LinearAllocator();

	void* Allocate(size_t size, size_t alignment = 1);

	template <class T>
	T* Allocate() {
		return reinterpret_cast<T*>(Allocate(sizeof(T), std::alignment_of<T>::value));
	}

	template <class T>
	T* AllocateArray(size_t numElements, size_t alignment = std::alignment_of<T>::value) {
		return reinterpret_cast<T*>(Allocate(sizeof(T) * numElements, alignment));
	}

	size_t GetCapacity() const {
		return m_capacity;
	}
	size_t GetSize() const {
		return m_freeSpace;
	}
	void* GetMemoryBuffer() const {
		return m_memoryBuffer;
	}
	void* GetOffset() const {
		return m_memoryOffset;
	}
	void   Rewind();
	void   Rewind(void* ptr);
	size_t GetPointerOffset(const void*) const;

private:
	Allocator* m_parentAllocator;
	void*      m_memoryBuffer;
	void*      m_memoryOffset;
	size_t     m_capacity;
	size_t     m_freeSpace;
};

inline size_t LinearAllocator::GetPointerOffset(const void* ptr) const {
	return reinterpret_cast<intptr_t>(ptr) - reinterpret_cast<intptr_t>(m_memoryBuffer);
}

} // namespace Typhoon
