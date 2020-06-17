#pragma once

namespace Typhoon {

struct StackBuffer {
	template <size_t S>
	explicit StackBuffer(char (&buffer)[S])
	    : buffer(buffer)
	    , size { S } {
	}
	char*  buffer = nullptr; // set this
	size_t size = 0;
	size_t offset = 0;
};

void* stackAlloc(StackBuffer& buffer, size_t size, size_t alignment);

void stackFree(StackBuffer& buffer, void* ptr);

} // namespace Typhoon
