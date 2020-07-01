#include "result.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <xutility>

namespace Typhoon::LuaBind {

namespace {

char tempBuffer[64];

}

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
	vsnprintf_s(tempBuffer, std::size(tempBuffer), std::size(tempBuffer) - 1, msg, args);
	errorMessage = tempBuffer;
	result = false;
}

} // namespace Typhoon::LuaBind
