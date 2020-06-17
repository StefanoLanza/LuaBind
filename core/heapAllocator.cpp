#include "heapAllocator.h"
#include <malloc.h>

namespace Typhoon {

HeapAllocator::HeapAllocator(size_t alignment)
    : Allocator(alignment) {
}

void* HeapAllocator::Alloc(size_t count, size_t alignment) {
	return _aligned_malloc(count, alignment);
}

void HeapAllocator::Free(void* ptr) {
	_aligned_free(ptr);
}

void* HeapAllocator::Realloc(void* ptr, size_t bytes, size_t alignment) {
	return _aligned_realloc(ptr, bytes, alignment);
}

} // namespace Typhoon
