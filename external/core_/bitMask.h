#pragma once

#include <cstdint>
#include <type_traits>

namespace Typhoon {

template <class StorageType_ = uint64_t>
struct BitMask {
	static_assert(std::is_unsigned_v<StorageType_>);
	using StorageType = StorageType_;
	StorageType value;
};

template <class Type>
inline void setBit(Type& bitMask, Type mask, bool value) {
	if (value) {
		bitMask |= mask;
	}
	else {
		bitMask &= (~mask);
	}
}

template <class Type>
inline bool getBit(Type bitMask, Type mask) {
	return (bitMask & mask) ? true : false;
}

template <class StorageType>
inline bool operator==(BitMask<StorageType> mask0, BitMask<StorageType> mask1) {
	return mask0.value == mask1.value;
}

template <class StorageType>
inline bool operator!=(BitMask<StorageType> mask0, BitMask<StorageType> mask1) {
	return ! (mask0 == mask1);
}

} // namespace Typhoon
