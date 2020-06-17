#include "registration.h"
#include <cassert>

namespace Typhoon::LUA::detail {

int beginNamespace(lua_State* ls, const char* name) {
	assert(name);
	Table namespaceTable = newtable(ls);
	assert(! globals(ls)[name]);
	globals(ls).Set(name, namespaceTable);
	lua_rawgeti(ls, LUA_REGISTRYINDEX, namespaceTable.GetReference().GetValue());
	return lua_gettop(ls);
}

} // namespace Typhoon::LUA::detail
