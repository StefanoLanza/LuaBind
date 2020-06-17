#include "stackAlloc.h"
#include "ptrUtil.h"
#include <cassert>

namespace Typhoon {

void* stackAlloc(StackBuffer& buffer, size_t size, size_t alignment) {
	assert(buffer.buffer);
	const size_t allocSize = size + (alignment - 1);
	assert(buffer.offset + allocSize <= buffer.size);
	// TODO save in the buffer the offset
	void* ptr = buffer.buffer + buffer.offset;
	buffer.offset += allocSize;
	return alignPointer(ptr, alignment);
}

void stackFree(StackBuffer& buffer, void* ptr) {
	buffer.offset = 0;
	// TODO decrease offset in first bytes of stackBuffer
}

} // namespace Typhoon
