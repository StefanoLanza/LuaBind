#include "allocator.h"

namespace Typhoon {

Allocator::Allocator(size_t alignment)
    : m_defaultAlignment(alignment) {
}

} // namespace Typhoon
