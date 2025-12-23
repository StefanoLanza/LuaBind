#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Typhoon {

uint32_t hash32(const char* data, size_t len);
uint32_t hash32(const char* str);
uint32_t hash32(std::string_view str);
uint64_t hash64(const char* data, size_t len);

template <class T>
uint32_t hash32(const T& obj) {
	return hash32(reinterpret_cast<const char*>(&obj), sizeof(obj));
}

template <class T>
uint64_t hash64(const T& obj) {
	return hash64(reinterpret_cast<const char*>(&obj), sizeof(obj));
}

} // namespace Typhoon
