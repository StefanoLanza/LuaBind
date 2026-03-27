// This example shows how to create a minimal C++ math library that can be used in Lua scripts

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void bindClasses(lua_State* ls);
void runExample(lua_State* ls);

// Vector
struct Vec3 {
	double x, y, z;
};

// Treat Vec3 as a lightweight object in Lua
// This means that Vec3 objects created by value in C++ are pushed as light user data pointers.
template <>
constexpr bool Typhoon::LuaBind::isLightweight<Vec3> = true;

// Custom new
Vec3* vec3New(const Vec3& v) {
	return new Vec3 { v };
}

void vec3Delete(Vec3* v) {
	delete v;
}

Vec3 vec3Set(float x, float y, float z) {
	return Vec3 { x, y, z };
}

Vec3 vec3Madd(const Vec3& v0, const Vec3& v1, float t) {
	return { v0.x + v1.x * t, v0.y + v1.y * t, v0.z + v1.z * t };
}

void vec3Store(Vec3* dst, const Vec3* src) {
	*dst = *src;
}

const char* initScript = R"(
	print ("Init")
	local pos = Vec3.set(0., 0., 0.)
	local vel = Vec3.set(1., 2., 3.)
	-- TODO global gameObj =
	gameObj = {
		pos = Vec3.new(pos),
		vel = Vec3.new(vel),
	}
)";

const char* updateScript = R"(
	print ("Update")

	local pos = gameObj.pos
	local vel = gameObj.vel

	local accel = Vec3.set(0., 8.91, 0.)
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

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "LuaBind version: " << LuaBind::getVersionString() << std::endl;
	Typhoon::HeapAllocator heapAllocator;
	lua_State* const       ls = LuaBind::createState(heapAllocator);
	bindClasses(ls);
	runExample(ls);
	LuaBind::closeState(ls);

	return 0;
}

void runExample(lua_State* ls) {
	using namespace LuaBind;

	// Initialize simulation
	if (Result res = doCommand(ls, initScript); ! res) {
		std::cout << res.error() << std::endl;
		return;
	}

	// Simulate update loop
	for (int i = 0; i < 10; ++i) {
		// Reset the memory buffer of temporary objects like Vec3
		LuaBind::resetAllocator(ls);

		// Simulate a frame
		if (Result res = doCommand(ls, updateScript); ! res) {
			std::cout << res.error() << std::endl;
		}
	}
}

void bindClasses(lua_State* ls) {
	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_CLASS(Vec3);
	LUA_NEW_OPERATOR(vec3New);
	LUA_DELETE_OPERATOR(vec3Delete);
	LUA_FUNCTION_RENAMED(vec3Set, set);
	LUA_FUNCTION_RENAMED(vec3Store, store);
	LUA_FUNCTION_RENAMED(vec3Madd, madd);
	LUA_GETTER(x, getX);
	LUA_GETTER(y, getY);
	LUA_GETTER(z, getZ);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
