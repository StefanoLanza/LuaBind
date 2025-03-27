#pragma once

#include <core/uncopyable.h>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

namespace Typhoon {

/**
 * @brief Allocator
 */
class Allocator {
public:
	static constexpr size_t defaultAlignment = alignof(void*);

	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;
	virtual void* realloc(void* ptr, size_t bytes, size_t alignment) = 0;

	// Helpers
	template <class T, bool zero = false>
	T* alloc() {
		static_assert(std::is_trivially_default_constructible_v<T>, "Use construct");
		void* ptr = alloc(sizeof(T), alignof(T));
		if (! ptr) {
			return nullptr;
		}
		if constexpr (zero) {
			std::memset(ptr, 0, sizeof(T));
		}
		return static_cast<T*>(ptr);
	}

	template <class T, bool zeroPOD = false>
	T* allocArray(size_t count) {
		static_assert(std::is_trivially_default_constructible_v<T>, "Use constructArray");
		void* ptr = alloc(sizeof(T) * count, alignof(T));
		if (! ptr) {
			return nullptr;
		}
		if constexpr (zeroPOD) {
			std::memset(ptr, 0, sizeof(T) * count);
		}
		return static_cast<T*>(ptr);
	}

	template <class T, class... ArgTypes>
	T* constructArray(size_t count, ArgTypes&&... args) {
		void* ptr = alloc(sizeof(T) * count, alignof(T));
		if (!ptr) {
			return nullptr;
		}
		for (size_t i = 0; i < count; ++i) {
			new (static_cast<std::byte*>(ptr) + sizeof(T) * i) T { std::forward<ArgTypes>(args)... } ;
		}
		return static_cast<T*>(ptr);
	}

	template <class T, class... ArgTypes>
	T* construct(ArgTypes&&... args) {
		void* ptr = alloc(sizeof(T), alignof(T));
		return ptr ? new (ptr) T { std::forward<ArgTypes>(args)... } : nullptr;
	}

	template <class T>
	void destroy(T* obj) {
		if (obj) {
			obj->~T();
			free(obj, sizeof *obj);
		}
	}
};

/**
 * @brief Heap allocator
 */
class HeapAllocator final : public Allocator {
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
	using Allocator::alloc;

	void          free(void* ptr, size_t size) override;
	void*         realloc(void* ptr, size_t bytes, size_t alignment) override;
	virtual void  rewind() = 0;
	virtual void  rewind(void* ptr) = 0;
	virtual void* getOffset() const = 0;
};

/**
 * @brief Buffer allocator
 */
class BufferAllocator final : public LinearAllocator, Uncopyable {
public:
	BufferAllocator(void* buffer, size_t bufferSize);
	BufferAllocator(Allocator& parentAllocator, size_t bufferSize);
	~BufferAllocator();

	using Allocator::alloc;

	void* alloc(size_t size, size_t alignment) override;
	void  rewind() override;
	void  rewind(void* ptr) override;
	void* getOffset() const override;
	void* getBuffer() const;

private:
	void*      buffer;
	Allocator* parentAllocator;
	void*      offset;
	size_t     bufferSize;
	size_t     freeSize;
};

inline void* BufferAllocator::getOffset() const {
	return offset;
}

inline void* BufferAllocator::getBuffer() const {
	return buffer;
}

/**
 * @brief Paged allocator
 */

class PagedAllocator final : public LinearAllocator {
public:
	PagedAllocator(Allocator& parentAllocator, size_t pageSize = defaultPageSize, size_t maxPages = 0);
	~PagedAllocator();

	using Allocator::alloc;

	void* alloc(size_t size, size_t alignment) override;
	void  rewind() override;
	void  rewind(void* ptr) override;
	void* getOffset() const override;

	static constexpr size_t defaultPageSize = 65536;

private:
	struct Page;
	Page* allocPage();
	void* allocFromPage(Page& page, size_t size, size_t alignment);

private:
	struct Page {
		Page*  next;
		Page*  prev;
		void*  buffer;
		void*  offset;
		size_t size;
	};
	Allocator* allocator;
	size_t     pageSize;
	size_t     maxPages;
	Page*      rootPage;
	Page*      currPage;
	size_t     pageCount;
};

} // namespace Typhoon
