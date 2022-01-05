#include "version.h"

namespace Typhoon::LuaBind {

Version getVersion() {
	return { TY_LUABIND_MAJOR_VERSION, TY_LUABIND_MINOR_VERSION, TY_LUABIND_PATCHLEVEL };
}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

const char* getVersionString() {
	return STR(TY_LUABIND_MAJOR_VERSION) "." STR(TY_LUABIND_MINOR_VERSION) "." STR(TY_LUABIND_PATCHLEVEL);
}

} // namespace Typhoon::LuaBind
