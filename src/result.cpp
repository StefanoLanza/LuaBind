#include "result.h"
#include <cassert>

namespace Typhoon::LuaBind {

Result::Result(bool result)
    : result(result)
    , errorMessage(nullptr) {
}

Result::Result(const char* message)
    : result(false)
    , errorMessage(message) {
	assert(message != nullptr);
}

} // namespace Typhoon::LuaBind
