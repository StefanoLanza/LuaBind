#pragma once

#include <cstdarg>

namespace Typhoon {

class Result {
public:
	explicit Result(bool result);
	explicit Result(const char* errorMessage, ...);
	bool GetResult() const {
		return result;
	}
	const char* getErrorMessage() const {
		return errorMessage;
	}
	explicit operator bool() const {
		return result;
	}
	void setErrorMessage(const char* msg, ...);

private:
	void setErrorMessage(const char* msg, va_list args);

private:
	bool        result;
	const char* errorMessage;
};

} // namespace Typhoon
