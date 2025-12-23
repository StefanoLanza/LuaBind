// This example shows how to access tables defined in Lua

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void runExample(lua_State* ls);
void printTable(const LuaBind::Table& table);

const char* script = R"(
	testTable = {
		name = "Stefano",
		surName = "Lanza",
		age = 46,
		male = true,
	}
)";

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "LuaBind version: " << LuaBind::getVersionString() << std::endl;
	Typhoon::HeapAllocator heapAllocator;
	lua_State* const       ls = LuaBind::createState(heapAllocator);
	runExample(ls);
	LuaBind::closeState(ls);

	return 0;
}

void runExample(lua_State* ls) {
	using namespace LuaBind;

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.error() << std::endl;
		return;
	}

	Table testTable = getGlobals(ls)["testTable"].asTable().value();
	if (! testTable) {
		return;
	}

	const char* name = testTable["name"].asString().value_or("");
	const char* surName = testTable["surName"].asString().value_or("");
	int  age = testTable["age"].asInt().value_or(0);
	bool male = testTable["male"].asBool().value_or(true);

	std::cout << std::boolalpha;
	std::cout << "Name:" << name << std::endl;
	std::cout << "Surname:" << surName << std::endl;
	std::cout << "Age:" << age << std::endl;
	std::cout << "Male:" << male << std::endl;

	// Insert element
	testTable.rawSet("city", "Mantova");
	const char* city = testTable["city"].asString().value_or("");
	std::cout << "City:" << city << std::endl;

	std::cout << std::endl;
	printTable(testTable);
}

void printValue(const LuaBind::Value& value) {
	switch (value.getType()) {
	case LUA_TSTRING: {
		std::cout << value.asString().value() << "[string]";
		break;
	}
	case LUA_TNUMBER: {
		std::cout << value.asInt().value() << "[number]";
		break;
	}
	case LUA_TBOOLEAN: {
		std::cout << value.asBool().value() << "[boolean]";
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
