#include "scopedAllocator.h"

namespace Typhoon {

struct ScopedAllocator::Finalizer {
	void (*destructor)(void* ptr);
	void*      obj;
	Finalizer* next;
	size_t     objSize;
};

ScopedAllocator::ScopedAllocator(Allocator& allocator)
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
		void*  obj = f->obj;
		size_t objSize = f->objSize;
		allocator.free(f, sizeof(Finalizer));
		allocator.free(obj, objSize);
	}
}

void* ScopedAllocator::alloc(size_t size, size_t alignment) {
	void* ptr = allocator.alloc(size, alignment);
	registerObject(ptr, size, nullptr);
	return ptr;
}

void ScopedAllocator::registerObject(void* obj, size_t objSize, Destructor destructor) {
	Finalizer* f = static_cast<Finalizer*>(allocator.alloc(sizeof(Finalizer), alignof(Finalizer)));
	f->destructor = destructor;
	f->obj = obj;
	f->objSize = objSize;
	f->next = finalizerHead;
	finalizerHead = f;
}

} // namespace Typhoon
