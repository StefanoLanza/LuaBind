#include "typeId.h"
#include <cassert>
#include <map>
#include <unordered_map>
#include <vector>

namespace Typhoon {

namespace {

struct TypeNameStorage {
	char string[32];
};

std::unordered_map<const void*, TypeNameStorage> idToName;
std::vector<std::pair<TypeNameStorage, TypeId>>  nameToId;

} // namespace

TypeName typeIdToName(TypeId typeId) {
	assert(typeId != nullTypeId);
	if (auto it = idToName.find(typeId.impl); it != idToName.end()) {
		return it->second.string;
	}
	return nullptr;
}

TypeId typeNameToId(const char* typeName) {
	assert(typeName);
	auto it = std::find_if(nameToId.begin(), nameToId.end(), [typeName](auto&& pair) { return ! strcmp(pair.first.string, typeName); });
	if (it != nameToId.end()) {
		return it->second;
	}
	return nullTypeId;
}

void registerTypeName(TypeId id, const char* typeName) {
	TypeNameStorage s;
	assert(strlen(typeName) < std::size(s.string));
#ifdef _MSC_VER
	strncpy_s(s.string, typeName, std::size(s.string));
#else
	strlcpy(s.string, typeName, std::size(s.string));
#endif
	auto r = idToName.insert({ id.impl, s });
	assert(r.second);
	nameToId.push_back({ s, id });
}

#if 0
constexpr auto intTypeId = getTypeId<int>();
constexpr auto floatTypeId = getTypeId<float>();
constexpr auto u64TypeId = getTypeId<unsigned __int64>();
constexpr auto size_tTypeId = getTypeId<size_t>();
static_assert(intTypeId != floatTypeId);
static_assert(size_tTypeId == u64TypeId);
#endif

} // namespace Typhoon
