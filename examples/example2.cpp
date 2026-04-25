// This example shows how to access in C++ objects defined in Lua

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void runExample(lua_State* ls);

const char* script = R"(
	local type = type
	local error = error
	
	global testObject = {
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
	lua_State* const       ls = LuaBind::createState(heapAllocator);
	runExample(ls);
	LuaBind::closeState(ls);

	return 0;
}

void runExample(lua_State* ls) {
	using namespace LuaBind;
	std::cout << std::boolalpha;

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.error() << std::endl;
		return;
	}

	Table test = getGlobals(ls)["testObject"].asTable().value();
	if (! test) {
		return;
	}

	const Object obj { ls, test.getReference() };

	if (ResultT<std::string> r = obj.callMethodRet<std::string>("getName"); r) {
		std::cout << "Name: " << r.value() << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}

	// Bad arguments
	if (auto r = obj.callMethod("setName", std::make_pair(0, 1)); ! r) {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethod("setName", "bird"); ! r) {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethodRet<std::string>("getName"); r) {
		std::cout << "New name: " << r.value() << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethodRet<float>("getSpeed"); r) {
		std::cout << "Speed: " << r.value() << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethod("setSpeed", 2.f); ! r) {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethodRet<float>("getSpeed"); r) {
		std::cout << "New speed: " << r.value() << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethodRet<bool>("canFly"); r) {
		std::cout << "Can fly?: " << r.value() << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}

	if (auto r = obj.callMethod("setPosition", 10.f, -20.f)) {
		float x = obj.callMethodRet<float>("getX").value();
		float y = obj.callMethodRet<float>("getY").value();
		std::cout << "New position: " << x << " " << y << std::endl;
	}
	else {
		std::cout << r.error() << std::endl;
	}
}
