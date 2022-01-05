#include "result.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <array>
//#include <xutility>

namespace Typhoon::LuaBind {

namespace {

char tempBuffer[256];

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
	vsnprintf(tempBuffer, std::size(tempBuffer), msg, args);
	errorMessage = tempBuffer;
	result = false;
}

} // namespace Typhoon::LuaBind
