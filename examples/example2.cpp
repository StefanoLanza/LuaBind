// This example shows how to access interact with objects defined in Lua

#include <include/luaBind.h>
#include <iostream>
#include <string>

void example(lua_State* ls);

const char* script = R"(
	testObject = {
		name = "Penguin",
		canFly = false,
		speed = 10.,
	}

	function testObject:getName()
		return self.name
	end

	function testObject:canFly()
		return self.canFly
	end

	function testObject:getSpeed()
		return self.speed
	end

	function testObject:setSpeed(newSpeed)
		self.speed = newSpeed
	end
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
	std::cout << std::boolalpha;

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
		return;
	}

	Table test = (Table)globals(ls)["testObject"];
	if (! test) {
		return;
	}

	Object obj(ls, test.getReference());

	std::string name;
	Result      r = obj.callMethodRet("getName", name);
	if (r) {
		std::cout << "Name: " << name << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}

	float speed;
	r = obj.callMethodRet("getSpeed", speed);
	if (r) {
		std::cout << "Speed: " << speed << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}

	r = obj.callMethod("setSpeed", speed * 2.f);
	if (! r) {
		std::cout << r.getErrorMessage() << std::endl;
	}

	r = obj.callMethodRet("getSpeed", speed);
	if (r) {
		std::cout << "New speed: " << speed << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}

	bool canFly;
	r = obj.callMethodRet("canFly", canFly);
	if (r) {
		std::cout << "Can fly?: " << canFly << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}
}
