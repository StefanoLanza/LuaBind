#pragma once

#include "allocator.h"

namespace Typhoon {

class HeapAllocator final : public Allocator {
public:
	explicit HeapAllocator(size_t alignment = 16);

	void* Alloc(size_t count, size_t alignment);
	void  Free(void* ptr);
	void* Realloc(void* ptr, size_t bytes, size_t alignment);
};

} // namespace Typhoon
