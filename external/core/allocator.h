#pragma once

#include <type_traits>

namespace Typhoon {

/**
 * @brief Allocator
 */
class Allocator {
public:
	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;
	virtual void* realloc(void* ptr, size_t bytes, size_t alignment) = 0;

	template <class T>
	T* alloc() {
		return static_cast<T*>(alloc(sizeof(T), alignof(T)));
	}

	template <class T>
	T* allocArray(size_t count) {
		static_assert(std::is_pod_v<T>);
		return static_cast<T*>(alloc(sizeof(T) * count, alignof(T)));	
	}

	static constexpr size_t defaultAlignment = 4;
};

/**
 * @brief Heap allocator
 */
class HeapAllocator : public Allocator {
public:
	void* alloc(size_t size, size_t alignment) override;
	void  free(void* ptr, size_t size) override;
	void* realloc(void* ptr, size_t bytes, size_t alignment) override;
};

/**
 * @brief Linear allocator
 */
class LinearAllocator : public Allocator {
public:
	LinearAllocator(char* buffer, size_t bufferSize, Allocator* backup);
	LinearAllocator(Allocator& allocator, size_t bufferSize, Allocator* backup);
	~LinearAllocator();

	void*  alloc(size_t size, size_t alignment) override;
	void   free(void* ptr, size_t size) override;
	void*  realloc(void* ptr, size_t bytes, size_t alignment) override;
	void   rewind();
	void   rewind(void* ptr);
	void*  getBuffer() const;
	void*  getOffset() const;
	size_t getPointerOffset(const void*) const;

private:
	char*      buffer;
	void*      offset;
	size_t     bufferSize;
	size_t     freeSize;
	Allocator* backup;
	Allocator* parent;
};

inline void* LinearAllocator::getOffset() const {
	return offset;
}

} // namespace Typhoon
