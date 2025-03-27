#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Typhoon {

uint32_t computeHash(const char* str, size_t len);
uint32_t computeHash(const char* str);
uint32_t hash32(const char* str);
uint32_t computeHash(std::string_view str);

template <class T>
uint32_t computeHash(const T& obj) {
	return computeHash(reinterpret_cast<const char*>(&obj), sizeof(obj));
}


} // namespace Typhoon
