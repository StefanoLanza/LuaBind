#include "typeId.h"
#include <cassert>
#include <unordered_map>

namespace Typhoon {

namespace {

struct TypeNameStorage {
	char string[32];
};

std::unordered_map<const void*, TypeNameStorage> idToName;
} // namespace

TypeName typeIdToName(TypeId typeId) {
	assert(typeId != nullTypeId);
	auto it = idToName.find(typeId.impl);
	if (it != idToName.end()) {
		return it->second.string;
	}
	return nullptr;
}

void registerTypeName(TypeId id, const char* typeName) {
	TypeNameStorage s;
	const auto      err = strncpy_s(s.string, std::size(s.string), typeName, _TRUNCATE);
	assert(err != STRUNCATE);
	auto r = idToName.insert({ id.impl, s });
	assert(r.second);
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
