#include "scopedAllocator.h"
#include <cassert>

namespace Typhoon {

struct ScopedAllocator::Finalizer {
	void (*destructor)(void* ptr);
	void*      obj;
	Finalizer* next;
	size_t     objSize;
};

ScopedAllocator::ScopedAllocator(LinearAllocator& allocator)
    : allocator(allocator)
    , finalizerHead(nullptr) {
}

ScopedAllocator::~ScopedAllocator() {
	for (Finalizer *f = finalizerHead, *next = nullptr; f; f = next) {
		if (f->destructor) {
			f->destructor(f->obj);
		}
		next = f->next;
		// Important: free finalizer first
		void* obj = f->obj;
		allocator.rewind(obj);
	}
}

void* ScopedAllocator::alloc(size_t size, size_t alignment) {
	void* ptr = allocator.alloc(size, alignment);
	registerObject(ptr, size, nullptr);
	return ptr;
}

void ScopedAllocator::registerObject(void* obj, size_t objSize, Destructor destructor) {
	Finalizer* f = static_cast<Finalizer*>(allocator.alloc(sizeof(Finalizer), alignof(Finalizer)));
	assert(f);
	f->destructor = destructor;
	f->obj = obj;
	f->objSize = objSize;
	f->next = finalizerHead;
	finalizerHead = f;
}

} // namespace Typhoon
