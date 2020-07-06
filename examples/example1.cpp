// This example shows how to access tables defined in Lua

#include <include/luaBind.h>
#include <iostream>
#include <string>

void example(lua_State* ls);
void printTable(const LuaBind::Table& table);

const char* script = R"(
	testTable = {
		name = "Stefano",
		surName = "Lanza",
		age = 41,
		male = true,
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

	Table testTable = (Table)globals(ls)["testTable"];
	if (! testTable) {
		return;
	}

	std::string name, surName, city;
	int         age;
	bool        male;
	testTable["name"].cast(name);
	testTable["surName"].cast(surName);
	testTable["age"].cast(age);
	testTable["male"].cast(male);

	std::cout << std::boolalpha;
	std::cout << "Name:" << name << std::endl;
	std::cout << "Surname:" << surName << std::endl;
	std::cout << "Age:" << age << std::endl;
	std::cout << "Male:" << male << std::endl;

	// Insert element
	testTable.rawSet("city", "Mantova");
	testTable["city"].cast(city);
	std::cout << "City:" << city << std::endl;

	std::cout << std::endl;
	printTable(testTable);
}

void printValue(const LuaBind::Value& value) {
	switch (value.getType()) {
	case LUA_TSTRING: {
		const char* str;
		value.cast(str);
		std::cout << str << "[string]";
		break;
	}
	case LUA_TNUMBER: {
		int n;
		value.cast(n);
		std::cout << n << "[number]";
		break;
	}
	case LUA_TBOOLEAN: {
		bool b;
		value.cast(b);
		std::cout << b << "[boolean]";
		break;
	}
	case LUA_TNIL:
		std::cout << "nil";
		break;
	case LUA_TLIGHTUSERDATA:
		std::cout << "ptr [light userData]";
		break;
	case LUA_TTABLE:
		std::cout << "table [table]";
		break;
	case LUA_TFUNCTION:
		std::cout << "function [function]";
		break;
	case LUA_TUSERDATA:
		std::cout << "ptr [userData]";
		break;
	default:
		break;
	};
}

void printTable(const LuaBind::Table& table) {
	std::cout << "Table contents" << std::endl;
	// Iterator
	for (const auto& keyValue : table) {
		printValue(keyValue.key);
		std::cout << " = ";
		printValue(keyValue.value);
		std::cout << std::endl;
	}
}
