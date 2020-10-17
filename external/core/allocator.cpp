#include "allocator.h"
#include "ptrUtil.h"

#include <cassert>
#include <memory>

namespace Typhoon {

void* HeapAllocator::alloc(size_t size, [[maybe_unused]] size_t alignment) {
#ifdef _MSC_VER
	return _aligned_malloc(size, alignment);
#else
	return ::malloc(size);
#endif
}

void HeapAllocator::free(void* ptr, size_t /*size*/) {
#ifdef _MSC_VER
	::_aligned_free(ptr);
#else
	::free(ptr);
#endif
}

void* HeapAllocator::realloc(void* ptr, size_t bytes, size_t alignment) {
#ifdef _MSC_VER
	return _aligned_realloc(ptr, bytes, alignment);
#else
	::realloc(ptr);
#endif
}

LinearAllocator::LinearAllocator(char* buffer, size_t bufferSize, Allocator* backup)
    : buffer(buffer)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize)
    , backup(backup)
    , parent(nullptr) {
}

LinearAllocator::LinearAllocator(Allocator& allocator, size_t bufferSize, Allocator* backup)
    : LinearAllocator(static_cast<char*>(allocator.alloc(bufferSize, 1)), bufferSize, backup) {
	parent = &allocator;
}

LinearAllocator::~LinearAllocator() {
	if (parent) {
		parent->free(buffer, bufferSize);
	}
}

void* LinearAllocator::alloc(size_t size, size_t alignment) {
	void* result = std::align(alignment, size, offset, freeSize);
	if (result) {
		offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(offset);
	}
	else if (backup) {
		result = backup->alloc(size, alignment);
	}
	assert(result);
	return result;
}

void LinearAllocator::free(void* ptr, size_t size) {
	if (ptr >= buffer && ptr < buffer + bufferSize) {
		if (static_cast<char*>(ptr) + size == offset) {
			offset = advancePointer(offset, -static_cast<ptrdiff_t>(size));
			freeSize += size;
		}
	}
	else if (backup) {
		backup->free(ptr, size);
	}
	else {
		assert(false); // Not allocated by this allocator or its backup
	}
}

void* LinearAllocator::realloc(void* ptr, size_t bytes, size_t alignment) {
	// TODO possibly reuse last allocation
	free(ptr, bytes);
	return alloc(bytes, alignment);
}

void LinearAllocator::rewind() {
	offset = buffer;
	freeSize = bufferSize;
}

void LinearAllocator::rewind(void* ptr) {
	if (ptr) {
		offset = ptr;
		freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(ptr);
		assert(freeSize <= bufferSize);
	}
}

void* LinearAllocator::getBuffer() const {
	return buffer;
}

size_t LinearAllocator::getPointerOffset(const void* ptr) const {
	return reinterpret_cast<intptr_t>(ptr) - reinterpret_cast<intptr_t>(buffer);
}

} // namespace Typhoon
