// This example shows how to access in C++ objects defined in Lua

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void runExample(lua_State* ls);

const char* script = R"(
	testObject = {
		name = "Penguin",
		canFly = false,
		speed = 10.,
		x = 0.,
		y = 0.,
	}

	function testObject:setName(newName)
		if type(newName) ~= "string" then
			return error("string expected")
		end
		self.name = newName
	end

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
		if type(newSpeed) ~= "number" then
			return error("number expected")
		end
		self.speed = newSpeed
	end

	function testObject:setPosition(x, y)
		if type(x) ~= "number" then
			return error("number expected")
		end
		if type(y) ~= "number" then
			return error("number expected")
		end
		self.x = x
		self.y = y
	end

	function testObject:getX()
		return self.x
	end

	function testObject:getY()
		return self.y
	end
)";

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "LuaBind version: " << LuaBind::getVersionString() << std::endl;
	Typhoon::HeapAllocator heapAllocator;
	lua_State* const ls = LuaBind::createState(heapAllocator);
	runExample(ls);
	LuaBind::closeState(ls);

	return 0;
}

void runExample(lua_State* ls) {
	using namespace LuaBind;
	std::cout << std::boolalpha;

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
		return;
	}

	Table test = static_cast<Table>(getGlobals(ls)["testObject"]);
	if (! test) {
		return;
	}

	Object obj(ls, test.getReference());
	Result r { false };

	std::string name;
	r = obj.callMethodRet("getName", name);
	if (r) {
		std::cout << "Name: " << name << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}

	// Bad arguments
	std::pair p{ 0, 1 };
	r = obj.callMethod("setName", p);
	if (! r) {
		std::cout << r.getErrorMessage() << std::endl;
	}

	name = "bird";
	r = obj.callMethod("setName", name);
	if (! r) {
		std::cout << r.getErrorMessage() << std::endl;
	}

	r = obj.callMethodRet("getName", name);
	if (r) {
		std::cout << "New name: " << name << std::endl;
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

	r = obj.callMethod("setSpeed", 2.f);
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

	bool canFly = false;
	r = obj.callMethodRet("canFly", canFly);
	if (r) {
		std::cout << "Can fly?: " << canFly << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}

	r = obj.callMethod("setPosition", 10.f, -20.f);
	if (r) {
		float x, y;
		obj.callMethodRet("getX", x);
		obj.callMethodRet("getY", y);
		std::cout << "New position: " << x << " " << y << std::endl;
	}
	else {
		std::cout << r.getErrorMessage() << std::endl;
	}
}
