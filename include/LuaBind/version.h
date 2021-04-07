#pragma once

#include <cstdint>

namespace Typhoon::LuaBind {

#define TY_LUABIND_MAJOR_VERSION 1
#define TY_LUABIND_MINOR_VERSION 0
#define TY_LUABIND_PATCHLEVEL    0

// This macro turns the version numbers into a numeric value
#define TY_LUABIND_VERSIONNUM(X, Y, Z) \
	((X)*1000 + (Y)*100 + (Z))

#define TY_LUABIND_COMPILEDVERSION \
	TY_LUABIND_VERSIONNUM(TY_LUABIND__MAJOR_VERSION, TY_LUABIND__MINOR_VERSION, TY_LUABIND__PATCHLEVEL)

struct Version {
	uint8_t major; /**< major version */
	uint8_t minor; /**< minor version */
	uint8_t patch; /**< update version */
};

Version getVersion();
const char* getVersionString();

} // namespace Typhoon::LuaBind
