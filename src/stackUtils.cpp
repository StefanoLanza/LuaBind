#include "stackUtils.h"
#include <cassert>

namespace Typhoon::LuaBind {

bool toString(lua_State* ls, int numArg, const char*& s) {
	if (! lua_isstring(ls, numArg)) {
		s = 0;
		return false;
	}
	const char* str = lua_tostring(ls, numArg);
	if (! str) {
		return false;
	}
	s = str;
	return true;
}

bool toString(lua_State* ls, int numArg, std::string& s) {
	if (! lua_isstring(ls, numArg)) {
		return false;
	}
	const char* str = lua_tostring(ls, numArg);
	if (! str) {
		return false;
	}
	s = std::string(str);
	return true;
}

bool toBool(lua_State* ls, int numArg, bool& b) {
	if (! lua_isboolean(ls, numArg)) {
		return false;
	}
	b = lua_toboolean(ls, numArg) ? true : false;
	return true;
}

bool toInteger(lua_State* ls, int numArg, int& i) {
	lua_Integer d = lua_tointeger(ls, numArg);
	if (d == 0 && ! lua_isnumber(ls, numArg)) { /* avoid extra test when d is not 0 */
		return false;
	}
	i = static_cast<int>(d);
	return true;
}

bool toFloat(lua_State* ls, int numArg, float& f) {
	if (! lua_isnumber(ls, numArg)) {
		return false;
	}
	f = static_cast<float>(lua_tonumber(ls, numArg));
	return true;
}

bool toDouble(lua_State* ls, int numArg, double& d) {
	if (! lua_isnumber(ls, numArg)) {
		return false;
	}
	d = static_cast<double>(lua_tonumber(ls, numArg));
	return true;
}

} // namespace Typhoon::LuaBind
