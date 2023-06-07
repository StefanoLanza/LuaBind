#include "allocator.h"
#include "ptrUtil.h"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <memory>

namespace Typhoon {

void* HeapAllocator::alloc(size_t size, [[maybe_unused]] size_t alignment) {
#ifdef _MSC_VER
	return _aligned_malloc(size, alignment);
#else
	return ::malloc(size);
#endif
}

void HeapAllocator::free(void* ptr, [[maybe_unused]] size_t size) {
#ifdef _MSC_VER
	::_aligned_free(ptr);
#else
	::free(ptr);
#endif
}

void* HeapAllocator::realloc(void* ptr, size_t bytes, [[maybe_unused]] size_t alignment) {
#ifdef _MSC_VER
	return _aligned_realloc(ptr, bytes, alignment);
#else
	return ::realloc(ptr, bytes);
#endif
}

void LinearAllocator::free([[maybe_unused]] void* ptr, [[maybe_unused]] size_t size) {
}

void* LinearAllocator::realloc([[maybe_unused]] void* ptr, size_t bytes, size_t alignment) {
	return alloc(bytes, alignment);
}

BufferAllocator::BufferAllocator(void* buffer, size_t bufferSize)
    : buffer(buffer)
    , parentAllocator(nullptr)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize) {
}

BufferAllocator::BufferAllocator(Allocator& parentAllocator, size_t bufferSize)
    : buffer(parentAllocator.alloc(bufferSize, parentAllocator.defaultAlignment))
    , parentAllocator(&parentAllocator)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize) {
}

void* BufferAllocator::alloc(size_t size, size_t alignment) {
	void* result = std::align(alignment, size, offset, freeSize);
	if (result) {
		offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(offset);
	}
	return result;
}

void BufferAllocator::rewind() {
	offset = buffer;
	freeSize = bufferSize;
}

void BufferAllocator::rewind(void* ptr) {
	if (ptr) {
		if (ptr >= buffer && ptr < static_cast<const char*>(buffer) + bufferSize) {
			offset = ptr;
			freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(ptr);
			assert(freeSize <= bufferSize);
		}
	}
}

PagedAllocator::PagedAllocator(Allocator& parentAllocator, size_t pageSize, size_t maxPages)
    : allocator(&parentAllocator)
    , pageSize(pageSize)
    , maxPages(maxPages ? maxPages : std::numeric_limits<size_t>::max())
    , rootPage(nullptr)
    , currPage(nullptr)
    , pageCount(0) {
	assert(pageSize > sizeof(Page));
}

PagedAllocator::~PagedAllocator() {
	for (Page* page = rootPage; page;) {
		Page* next = page->next; // Fetch before freeing page
		allocator->free(page->buffer, pageSize);
		page = next;
	}
}

void* PagedAllocator::alloc(size_t size, size_t alignment) {
	if (size > pageSize - sizeof(Page)) {
		return nullptr;
	}
	if (! rootPage) {
		rootPage = allocPage();
		if (! rootPage) {
			return nullptr;
		}
		currPage = rootPage;
	}

	for (Page* page = currPage; page != nullptr; page = page->next) {
		currPage = page;
		if (void* result = allocFromPage(*page, size, alignment); result) {
			return result;
		}
	}

	Page* newPage = allocPage();
	if (newPage) {
		newPage->prev = currPage;
		currPage->next = newPage;
		currPage = newPage;
		if (void* result = allocFromPage(*newPage, size, alignment); result) {
			return result;
		}
	}
	return nullptr;
}

void PagedAllocator::rewind() {
	for (Page* page = currPage; page; page = page->prev) {
		page->offset = advancePointer(page->buffer, sizeof(Page));
	}
	currPage = rootPage;
}

void PagedAllocator::rewind(void* ptr) {
	if (ptr) {
		for (Page* page = currPage; page != nullptr; page = page->prev) {
			if (ptr >= page->buffer && ptr < static_cast<const char*>(page->buffer) + page->size) {
				page->offset = ptr;
				currPage = page;
				break;
			}
		}
	}
}

inline void* PagedAllocator::getOffset() const {
	return currPage ? currPage->offset : nullptr;
}

PagedAllocator::Page* PagedAllocator::allocPage() {
	if (pageCount >= maxPages) {
		return nullptr;
	}
	void* buffer = allocator->alloc(pageSize, Allocator::defaultAlignment);
	if (buffer) {
		Page newPage;
		newPage.next = nullptr;
		newPage.prev = nullptr;
		newPage.buffer = buffer;
		newPage.offset = advancePointer(buffer, sizeof(Page));
		newPage.size = pageSize - sizeof(Page);
		std::memcpy(buffer, &newPage, sizeof newPage);
		++pageCount;
	}
	return static_cast<Page*>(buffer);
}

void* PagedAllocator::allocFromPage(Page& page, size_t size, size_t alignment) {
	size_t freeSize = reinterpret_cast<uintptr_t>(page.buffer) + page.size - reinterpret_cast<uintptr_t>(page.offset);
	assert(freeSize <= page.size - sizeof(Page));
	void* result = std::align(alignment, size, page.offset, freeSize);
	if (result) {
		page.offset = advancePointer(result, size);
	}
	return result;
}

} // namespace Typhoon
