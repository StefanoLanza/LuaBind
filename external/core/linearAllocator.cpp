#include "linearAllocator.h"
#include "allocator.h"
#include <cassert>
#include <memory>

namespace Typhoon {

LinearAllocator::LinearAllocator(Allocator& allocator, size_t capacity)
    : m_parentAllocator(&allocator)
    , m_memoryBuffer(allocator.Alloc(capacity))
    , m_memoryOffset(m_memoryBuffer)
    , m_capacity(capacity)
    , m_freeSpace(capacity) {
	assert(m_memoryBuffer);
	assert(capacity);
}

LinearAllocator::LinearAllocator(size_t capacity, size_t alignment)
    : m_parentAllocator(nullptr)
    , m_memoryBuffer(_aligned_malloc(capacity, alignment))
    , m_memoryOffset(m_memoryBuffer)
    , m_capacity(capacity)
    , m_freeSpace(capacity) {
	assert(m_memoryBuffer);
	assert(capacity);
}

LinearAllocator::~LinearAllocator() {
	if (m_parentAllocator) {
		m_parentAllocator->Free(m_memoryBuffer);
	}
	else {
		_aligned_free(m_memoryBuffer);
	}
}

void* LinearAllocator::Allocate(size_t size, size_t alignment) {
	void* const result = std::align(alignment, size, m_memoryOffset, m_freeSpace);
	if (result) {
		m_memoryOffset = static_cast<uint8_t*>(result) + size;
		m_freeSpace = reinterpret_cast<uintptr_t>(m_memoryBuffer) + m_capacity - reinterpret_cast<uintptr_t>(m_memoryOffset);
	}
	return result;
}

void LinearAllocator::Rewind() {
	m_memoryOffset = m_memoryBuffer;
	m_freeSpace = m_capacity;
}

void LinearAllocator::Rewind(void* ptr) {
	if (ptr) {
		m_memoryOffset = ptr;
		m_freeSpace = reinterpret_cast<uintptr_t>(m_memoryBuffer) + m_capacity - reinterpret_cast<uintptr_t>(ptr);
		assert(m_freeSpace <= m_capacity);
	}
}

} // namespace Typhoon
