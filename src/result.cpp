#include "result.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace Typhoon::LuaBind {

Result::Result(bool result)
    : result(result)
    , errorMessage(nullptr) {
}

Result::Result(const char* errorMessage, ...)
    : result(false) {
	assert(errorMessage != nullptr);
	va_list args;
	va_start(args, errorMessage);
	setErrorMessage(errorMessage, args);
	va_end(args);
}

void Result::setErrorMessage(const char* msg, va_list args) {
	vsnprintf(messageBuffer, std::size(messageBuffer), msg, args);
	errorMessage = messageBuffer;
	result = false;
}

} // namespace Typhoon::LuaBind
