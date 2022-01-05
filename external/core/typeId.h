#pragma once

#include <type_traits>
#include <functional> // hash

namespace Typhoon {

// struct uniquely representing a C++ type
struct TypeId {
	const void* impl;

	operator bool() const {
		return impl != nullptr;
	}
};

inline constexpr TypeId nullTypeId { nullptr };
inline constexpr bool   operator==(TypeId a, TypeId b) {
    return a.impl == b.impl;
}
inline constexpr bool operator!=(TypeId a, TypeId b) {
	return ! (a.impl == b.impl);
}

template <typename T>
struct type_id_ptr {
	// NOTE: don't make id const to prevent MSVC optimization with /OPT:IFC
	// See
	// https://stackoverflow.com/questions/41868077/is-it-safe-to-use-the-address-of-a-static-local-variable-within-a-function-templ
	inline static char           id = 0;
	inline static constexpr bool isReference = std::is_reference_v<T>;
};

template <typename T>
constexpr TypeId getTypeId() noexcept {
	using BareType = std::remove_cv_t<T>; // remove const and volatile
	return TypeId { &type_id_ptr<BareType>::id };
}

// This can be specialized, e.g. for polymorphic objects
template <typename T>
constexpr TypeId getTypeId(const T* /*dummy*/) noexcept {
	static_assert(! std::is_pointer_v<T>);
	return getTypeId<T>();
}

using TypeName = const char*;

//
TypeName typeIdToName(TypeId typeId);

//
TypeId typeNameToId(const char* typeName);

//
void registerTypeName(TypeId typeId, const char* className);

template <typename T>
TypeName typeName() {
	return typeIdToName(getTypeId<T>());
}

// This can be specialized, e.g. for polymorphic objects
template <typename T>
TypeName typeName(const T* dummy) {
	static_assert(! std::is_pointer_v<T>);
	return typeName(getTypeId(dummy));
}

} // namespace Typhoon

template <>
struct std::hash<Typhoon::TypeId> {
	std::size_t operator()(const Typhoon::TypeId& typeId) const {
		return std::hash<const void*>{}(typeId.impl);
	}
};
