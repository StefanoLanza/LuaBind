// This example shows how to create a minimal C++ math library that can be used in Lua scripts

#include <luaBind/luaBind.h>
#include <luaBind/version.h>

#include <core/allocator.h>
#include <core/poolAllocator.h>
#include <iostream>
#include <string>

void bindClasses(lua_State* ls);
void runExample(lua_State* ls);

// Vector
struct Vec3 {
	double x, y, z;
};

Typhoon::PoolAllocator<Vec3>* gVec3Allocator = nullptr;

// Treat Vec3 as a lightweight object in Lua
// This means that Vec3 objects created by value in C++ are pushed as light user data pointers.
template <>
constexpr bool Typhoon::LuaBind::isLightweight<Vec3> = true;

// Custom new
Vec3* vec3New(float x, float y, float z) {
	return gVec3Allocator->create(x, y, z);
}

void vec3Delete(Vec3* v) {
	gVec3Allocator->destroy(v);
}

Vec3 vec3Set(float x, float y, float z) {
	return { x, y, z };
}

Vec3 vec3Madd(const Vec3& v0, const Vec3& v1, float t) {
	return { v0.x + v1.x * t, v0.y + v1.y * t, v0.z + v1.z * t };
}

void vec3Store(Vec3* dst, const Vec3* src) {
	*dst = *src;
}

Vec3 vec3Scale(Vec3 v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}

Vec3 operator+(Vec3 a, Vec3 b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 operator-(Vec3 a, Vec3 b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

const char* initScript = R"(
	print ("Init")
	-- Stress test allocator
	for i = 1, 1024 do
		local v = Vec3.new(0., 0., 0.)
	end

	-- TODO global gameObj =
	gameObj = {
		pos = Vec3.new(0., 0., 0.),
		vel = Vec3.new(1., 2., 3.),
		angles = Vec3.new(0., 0., 0.),
	}
)";

const char* updateScript = R"(
	print ("Update")

	local dt = 0.1 -- elapsed time in seconds

	local pos = gameObj.pos
	local vel = gameObj.vel
	local newAngles = Vec3.add(gameObj.angles, Vec3.scale(Vec3.set(0.1, 0.2, 0.3), dt)) -- rotate

	local accel = Vec3.set(0., 8.91, 0.)
	-- Euler integration
	local newVel = Vec3.madd(vel, accel, dt * dt)
	local newPos = Vec3.madd(pos, newVel, dt)
	print(string.format("  pos: %.3f   %.3f   %.3f",  Vec3.getX(newPos), Vec3.getY(newPos), Vec3.getZ(newPos)));
	print(string.format("  vel: %.3f   %.3f   %.3f",  Vec3.getX(newVel), Vec3.getY(newVel), Vec3.getZ(newVel)));
	print(string.format("  angles: %.3f   %.3f   %.3f",  Vec3.getX(newAngles), Vec3.getY(newAngles), Vec3.getZ(newAngles)));

	-- Store computed values in the game object
	Vec3.store(gameObj.pos, newPos)
	Vec3.store(gameObj.vel, newVel)
	Vec3.store(gameObj.angles, newAngles)
)";

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "LuaBind version: " << LuaBind::getVersionString() << std::endl;
	Typhoon::HeapAllocator       heapAllocator;
	Typhoon::PoolAllocator<Vec3> vec3Allocator { heapAllocator, 1024 };
	gVec3Allocator = &vec3Allocator;
	lua_State* const ls = LuaBind::createState(heapAllocator);
	bindClasses(ls);
	runExample(ls);
	vec3Allocator.clear();
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
	LUA_OBJ_ALLOCATOR(vec3New, vec3Delete);
	LUA_FUNCTION_RENAMED(vec3Set, set);
	LUA_FUNCTION_RENAMED(vec3Store, store);
	LUA_FUNCTION_RENAMED(vec3Madd, madd);
	LUA_FUNCTION_RENAMED(vec3Scale, scale);
	LUA_FREE_OPERATOR(add, +);
	LUA_FREE_OPERATOR(sub, -);
	LUA_GETTER(x, getX);
	LUA_GETTER(y, getY);
	LUA_GETTER(z, getZ);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
