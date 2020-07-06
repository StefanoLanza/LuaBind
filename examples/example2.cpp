// This example shows how to access tables defined in Lua

#include <include/luaBind.h>
#include <string>
#include <iostream>

void example(lua_State* ls);

const char* script = R"(
	testTable = {
		name = "Stefano",
		surName = "Lanza",
	}
)";

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	lua_State* const ls = LuaBind::createState(8192);
	example(ls);
	LuaBind::closeState(ls);

	return 0;
}

void example(lua_State* ls) {
	using namespace LuaBind;
	const AutoBlock autoblock(ls);

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
		return;
	}

	/*Table table = newtable(ls);

	const int numIndices = 101;
	for (int i = 0; i < numIndices; ++i) {
		table.rawSeti(i, i * i); // table[i] = i * i
	}

	const int key = 10;
	const int value = 20;
	table.set(key, value);
	const int ret_value = (int)table[key];
	*/
	if (Table test = (Table)globals(ls)["testTable"]; test) {
		std::string name, surName;
		test["name"].cast(name);
	}
}
