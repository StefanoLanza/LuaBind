// This example shows how to create a minimal C++ math library that can be used in Lua scripts

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void bind(lua_State* ls);
void runExample(lua_State* ls);

// Vector
struct Vec3 {
	double x, y, z;
};

// Treat Vec3 as a temporary lightweight object in Lua
// This means that Vec3 objects created by Lua are are allocated in C++ from a temporary memory buffer and treated as light user data pointers.
// This buffer is reset every frame. For permanent storage, you have to manually box and unbox Vec3 objects. See the example scripts
template <>
class LuaBind::Wrapper<Vec3> : public LuaBind::Lightweight<Vec3> {};

// Custom new
Vec3 newVec3(float x, float y, float z) {
	return Vec3 { x, y, z };
}

Vec3 vec3Madd(const Vec3& v0, const Vec3& v1, float t) {
	return { v0.x + v1.x * t, v0.y + v1.y * t, v0.z + v1.z * t };
}

const char* initScript = R"(
	print ("Init")
	local pos = Vec3.new(0., 0., 0.)
	local vel = Vec3.new(1., 2., 3.)
	gameObj = {
		pos = Vec3.box(pos),
		vel = Vec3.box(vel),
	}
)";

const char* updateScript = R"(
	print ("Update")

	-- Unbox position and vel
	local pos = Vec3.retrieve(gameObj.pos)
	local vel = Vec3.retrieve(gameObj.vel)

	local accel = Vec3.new(0., 8.91, 0.)
	local dt = 0.1 -- elapsed time in seconds
	-- Euler integration
	local newVel = Vec3.madd(vel, accel, dt * dt)
	local newPos = Vec3.madd(pos, newVel, dt)
	print(string.format("  pos: %.3f   %.3f   %.3f",  Vec3.getX(newPos), Vec3.getY(newPos), Vec3.getZ(newPos)));
	print(string.format("  vel: %.3f   %.3f   %.3f",  Vec3.getX(newVel), Vec3.getY(newVel), Vec3.getZ(newVel)));

	-- Store computed values in the game object
	Vec3.store(gameObj.pos, newPos)
	Vec3.store(gameObj.vel, newVel)
)";

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "LuaBind version: " << LuaBind::getVersionString() << std::endl;
	Typhoon::HeapAllocator heapAllocator;
	lua_State* const ls = LuaBind::createState(heapAllocator);
	bind(ls);
	runExample(ls);
	LuaBind::closeState(ls);

	return 0;
}

void runExample(lua_State* ls) {
	using namespace LuaBind;

	// Initialize simulation
	if (Result res = doCommand(ls, initScript); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
		return;
	}

	// Simulate update loop
	for (int i = 0; i < 10; ++i) {

		// Reset the memory buffer of temporary objects like Vec3
		LuaBind::newFrame(ls);

		// Simulate a frame
		if (Result res = doCommand(ls, updateScript); ! res) {
			std::cout << res.getErrorMessage() << std::endl;
		}
	}
}

void bind(lua_State* ls) {
	using namespace LuaBind;

	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_CLASS(Vec3);
	LUA_NEW_OPERATOR(newVec3);
	LUA_FUNCTION_RENAMED(vec3Madd, madd);
	LUA_GETTER(x, getX);
	LUA_GETTER(y, getY);
	LUA_GETTER(z, getZ);
	// LUA_BOX_OPERATOR();
	LUA_END_CLASS();

	LUA_END_BINDING();
}
