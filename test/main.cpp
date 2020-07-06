#include "testClass.h"
#include <core/typedVoidPtr.h>
#include <extras/voidPtrWrapper.h>
#include <include/luaBind.h>

#include <array>
#include <string>
#include <vector>

#define CATCH_CONFIG_RUNNER
#include <Catch-master/single_include/catch2/catch.hpp>

lua_State* g_ls = nullptr;

void globalFunction() {
	std::cout << "Global function" << std::endl;
}
void globalFunction2(std::string str) {
	std::cout << "Global function2" << str << std::endl;
}

// Lightweight math objects
struct Vec3 {
	double x, y, z;
};

struct Quat {
	double x, y, z, w;
};

// Custom new
int newVec3(lua_State* ls) {
	Vec3* v = LuaBind::newTemporary<Vec3>();
	// todo merge ?
	lua_pushlightuserdata(ls, v);
#if TY_LUABIND_TYPE_SAFE
	LuaBind::detail::registerPointerType(v);
#endif
	v->x = lua_isnumber(ls, 2) ? lua_tonumber(ls, 2) : 0;
	v->y = lua_isnumber(ls, 3) ? lua_tonumber(ls, 3) : 0;
	v->z = lua_isnumber(ls, 4) ? lua_tonumber(ls, 4) : 0;
	return 1;
}

Vec3 add(const Vec3& v0, const Vec3& v1) {
	return { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z };
}

void setIdentity(Quat& q) {
	q = { 0., 0., 0., 1. };
}

// Treat Vec3 as a temporary object in Lua
template <>
class LuaBind::Wrapper<Vec3> : public LuaBind::WrapperAsTemporary<Vec3> {};

void bindTestClasses(lua_State* ls);

int __cdecl main(int argc, char* argv[]) {
	lua_State* const ls = LuaBind::createState(8192);
	g_ls = ls;
	bindTestClasses(ls);
	const int result = Catch::Session().run(argc, argv);
	LuaBind::closeState(ls);
	return result;
}

TEST_CASE("Globals") {
	using namespace LuaBind;
	lua_State*      ls = g_ls;
	const AutoBlock autoblock(ls);
	CHECK(doCommand(ls, "Globals.globalFunction()"));
	CHECK(doCommand(ls, "Globals.globalFunction2('ciao')"));
}

TEST_CASE("Table") {
	using namespace LuaBind;
	lua_State*      ls = g_ls;
	const AutoBlock autoblock(ls);
	Table           table = newtable(ls);

	SECTION("indices") {
		CHECK(lua_gettop(ls) == 0);
		const int numIndices = 101;
		for (int i = 0; i < numIndices; ++i) {
			table.rawSeti(i, i * i); // table[i] = i * i
		}
		CHECK(lua_gettop(ls) == 0);
		for (int i = 0; i < numIndices; ++i) {
			CHECK(table.hasElement(i));
		}
		CHECK(lua_gettop(ls) == 0);
		CHECK(table.getCount() == numIndices);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("int int") {
		const int key = 10;
		const int value = 20;
		table.set(key, value);
		CHECK(lua_gettop(ls) == 0);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TNUMBER);
		const int ret_value = (int)table[key];
		CHECK(ret_value == value);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("uint double") {
		const unsigned int key = 40;
		const double       value = 3.f;
		table.set(key, value);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TNUMBER);
		const double ret_value = (double)table[key];
		CHECK(ret_value == value);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("ptr ptr") {
		const char* key = "voidptr";
		void* const value = &key;
		table.set(key, value);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TLIGHTUSERDATA);
		void* res = (void*)table[key];
		CHECK(res == value);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("cstr cstr") {
		const char* key = "key";
		const char* value = "value 0";
		table.set(key, value);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TSTRING);
		std::string ret_value;
		CHECK(table[key].cast(ret_value));
		CHECK(ret_value == value);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("char int") {
		const char key = 'c';
		const int  value = 10;
		table.set(key, value);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TNUMBER);
		int ret_value = 0;
		CHECK(table[key].cast(ret_value));
		CHECK(ret_value == value);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("uchar float") {
		const unsigned char key = 'u';
		const float         value = 3.14f;
		table.set(key, value);
		CHECK_FALSE(table[key].isNil());
		CHECK(table[key].getType() == LUA_TNUMBER);
		float ret_value = 0;
		CHECK(table[key].cast(ret_value));
		CHECK(ret_value == value);
		CHECK(lua_gettop(ls) == 0);
	}
}

TEST_CASE("std") {
	using namespace LuaBind;
	lua_State* ls = g_ls;
	AutoBlock  autoblock(ls);

	SECTION("std::pair") {
		const int idx = lua_gettop(ls);
		using pair_type = std::pair<std::string, int>;

		const pair_type pair { "value", 10 };
		push(ls, pair);

		const pair_type tmp = get<pair_type>(ls, idx + 1);
		CHECK(pair == tmp);
	}

	SECTION("std::vector") {
		const int idx = lua_gettop(ls);
		using vec_type = std::vector<std::string>;

		const vec_type vec1 { "stefano", "claudio", "cristiana", "manlio", "nicoletta" };
		push(ls, vec1);
		const vec_type vec2 = get<vec_type>(ls, idx + 1);
		CHECK(vec1 == vec2);
	}

	SECTION("std::array") {
		using arrayType = std::array<std::string, 5>;
		const arrayType stringArray = { "stefano", "claudio", "cristiana", "nico", "manlio" };
		const int       idx = lua_gettop(ls);
		CHECK(push(ls, stringArray) == 1);

		const arrayType testArray = get<arrayType>(ls, idx + 1);
		CHECK(stringArray == testArray);
	}
}

TEST_CASE("Properties") {
	using namespace LuaBind;
	lua_State* ls = g_ls;
	AutoBlock  autoblock(ls);
	if (globals(ls)["ptest"]) {
		Table ptest = (Table)globals(ls)["ptest"];
		REQUIRE(ptest);

		Table properties = (Table)ptest["p"];
		REQUIRE(properties);
	}
}

TEST_CASE("VoidPtr") {
	using namespace LuaBind;
	lua_State* ls = g_ls;
	AutoBlock  autoBlock(ls);
	Biped      biped;
	biped.SetName("Biped");
	const Typhoon::VoidPtr voidPtr = Typhoon::MakeVoidPtr(&biped);
	push(ls, voidPtr);
}

#if 0
TEST_CASE("GC")
{
	AutoBlock autoblock(ls);

	std::shared_ptr<int> obj(new int);
	Push(ls, obj);

	// Collect garbage
	lua_gc(ls, LUA_GCCOLLECT, 0);
	while (! lua_gc(ls, LUA_GCSTEP, 2000))
	{
	}
	CHECK(obj.use_count() == 1);
}
#endif

TEST_CASE("Class") {
	using namespace LuaBind;
	lua_State* ls = g_ls;

	SECTION("base class") {
		// Registration as user data
		SECTION("binding C++ object as full user data") {
			auto            bart = std::make_unique<GameObject>();
			const Reference ref(registerObjectAsUserData(ls, bart.get()));
			REQUIRE(ref.isValid());
			CHECK(registry(ls)[bart.get()].isNil() == false);
			globals(ls).set("bart", bart.get());
			CHECK(globals(ls)["bart"].getType() == LUA_TUSERDATA);
#if TY_LUABIND_TYPE_SAFE
			{
				AutoBlock autoblock(ls);
				push(ls, ref);
				CHECK(detail::checkType<GameObject>(ls, lua_gettop(ls)));
			}
#endif
			CHECK(doCommand(ls, "bart:SetA(10)"));
			CHECK(bart->GetA() == 10);
			CHECK(doCommand(ls, "bart:SetName('Bart')"));
			CHECK(bart->GetName() == "Bart");
			if (ref) {
				unregisterObject(ls, ref);
			}
			CHECK(registry(ls)[bart.get()].isNil());
			globals(ls).set("bart", nil);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("binding C++ object as table") {
			// Registration as table
			auto            fred = std::make_unique<GameObject>();
			const Reference ref(registerObjectAsTable(ls, fred.get()));
			REQUIRE(ref.isValid());
			CHECK(registry(ls)[fred.get()].isNil() == false);
			globals(ls).set("fred", fred.get());
			CHECK(globals(ls)["fred"].getType() == LUA_TTABLE);
#if TY_LUABIND_TYPE_SAFE
			{
				AutoBlock autoblock(ls);
				push(ls, ref);
				CHECK(detail::checkType<GameObject>(ls, lua_gettop(ls)));
			}
#endif
			CHECK(doCommand(ls, "fred:SetA(20)"));
			CHECK(fred->GetA() == 20);
			CHECK(doCommand(ls, "fred:SetName('Fred')"));
			CHECK(fred->GetName() == "Fred");
			unregisterObject(ls, ref);
			CHECK(registry(ls)[fred.get()].isNil() == true);
			globals(ls).set("fred", nil);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("binding C++ object as light user data") {
			auto            barney = std::make_unique<GameObject>();
			const Reference ref = registerObjectAsLightUserData(ls, barney.get());
			REQUIRE(ref.isValid());
			globals(ls).set("barney", barney.get());
			CHECK(globals(ls)["barney"].getType() == LUA_TLIGHTUSERDATA);

			CHECK(doCommand(ls, "GameObject.SetA(barney, 20)"));
			CHECK(barney->GetA() == 20);
			CHECK(doCommand(ls, "GameObject.SetName(barney, 'Barney')"));
			CHECK(barney->GetName() == "Barney");

			unregisterObject(ls, ref);
			globals(ls).set("barney", nil);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("creating C++ object in Lua") {
			CHECK(doCommand(ls, "obj = GameObject()"));
			CHECK(doCommand(ls, "GameObject.SetA(obj, 20)"));
			CHECK(doCommand(ls, "GameObject.SetName(obj, 'Stanley')"));
			CHECK(doCommand(ls, "GameObject.setState(obj, 0)"));
			CHECK(doCommand(ls, "GameObject.getState(obj)"));
			CHECK(doCommand(ls, "obj = nil"));
			CHECK(lua_gettop(ls) == 0);
		}
	}

	SECTION("sub class") {
		auto            biped = std::make_unique<Biped>();
		const Reference ref = registerObjectAsUserData(ls, biped.get());
		REQUIRE(ref.isValid());
		globals(ls).set("subobj", ref);
		const Biped* tmp2 = (const Biped*)globals(ls)["subobj"];
		REQUIRE(tmp2);
		CHECK(globals(ls)["subobj"].getType() == LUA_TUSERDATA);

#if TY_LUABIND_TYPE_SAFE
		{
			AutoBlock autoblock(ls);
			push(ls, ref);
			CHECK(detail::checkType<GameObject>(ls, lua_gettop(ls)));
			CHECK(detail::checkType<Biped>(ls, lua_gettop(ls)));
		}
#endif

		doCommand(ls, "subobj:SetA(20)");
		CHECK(biped->GetA() == 20);
		doCommand(ls, "subobj:SetC(30)");
		CHECK(biped->GetC() == 30.f);
		doCommand(ls, "subobj:SetName('Homer')");
		CHECK(biped->GetNameRef() == "Homer");

		unregisterObject(ls, ref);
		globals(ls).set("subobj", nil);
		CHECK(lua_gettop(ls) == 0);
	}

	SECTION("struct") {
		Human human;
		human.energy = 10;
		const Reference ref = registerObjectAsUserData(ls, &human);
		REQUIRE(ref.isValid());
		globals(ls).set("human", ref);

		CHECK_FALSE(globals(ls)["human"].isNil());

		CHECK(doCommand(ls, "human:SetName('Rocky')"));
		CHECK(human.GetName() == "Rocky");

		doCommand(ls, "energy = human:GetEnergy()");
		const float energy = (float)globals(ls)["energy"];
		CHECK(energy == human.energy);

		doCommand(ls, "energy = human:SetEnergy(20)");
		CHECK(20 == human.energy);

		// C API
		doCommand(ls, "Human.AddEnergy(human, 10.)");
	}
#if 0
	{
		Object luaobj(ls, ref);
	bool res = luaobj.CallMethod("GetA", 1);
	CHECK(res);
	}
{
	Object luaobj(ls, ref);
	bool res = luaobj.CallMethod("GetA", 1);
	CHECK(res);
}
#endif
	SECTION("temporaries") {
		CHECK(doCommand(ls, "vec0 = Vec3(0., 1., 2.)"));
		CHECK(doCommand(ls, "vec1 = Vec3(3., 4., 5.)"));
		CHECK(doCommand(ls, "vec2 = Vec3.add(vec0, vec1)"));
		const Vec3* vptr = static_cast<const Vec3*>(globals(ls)["vec0"]);
		REQUIRE(vptr);
		CHECK(vptr->x == 0.);
		CHECK(vptr->y == 1.);
		CHECK(vptr->z == 2.);
		const Vec3* v1ptr = static_cast<const Vec3*>(globals(ls)["vec1"]);
		REQUIRE(v1ptr);
		CHECK(v1ptr->x == 3.);
		CHECK(v1ptr->y == 4.);
		CHECK(v1ptr->z == 5.);
		const Vec3* v2ptr = static_cast<const Vec3*>(globals(ls)["vec2"]);
		REQUIRE(v2ptr);
		CHECK(v2ptr->x == 3.);
		CHECK(v2ptr->y == 5.);
		CHECK(v2ptr->z == 7.);
#if TY_LUABIND_TYPE_SAFE
		// Type safefy
		// const Quat* q = static_cast<const Quat*>(globals(ls)["vec0"]);
		// CHECK(q == nullptr);
		// CHECK(! doCommand(ls, "Quat.setIdentity(vec0)"));
#endif
	}
}

void bindTestClasses(lua_State* ls) {
	using namespace LuaBind;

	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_NAMESPACE(Globals);
	LUA_ADD_FREE_FUNCTION(globalFunction);
	LUA_ADD_FREE_FUNCTION(globalFunction2);
	LUA_END_NAMESPACE();

	LUA_BEGIN_CLASS(GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(SetName);
	LUA_ADD_METHOD(GetNameRef);
	LUA_ADD_METHOD(GetName);
	LUA_ADD_METHOD(SetA);
	LUA_ADD_METHOD(SetB);
	LUA_ADD_METHOD(GetA);
	LUA_ADD_METHOD(GetB);
	LUA_ADD_METHOD(getState);
	LUA_ADD_METHOD(setState);
	LUA_ADD_OVERLOADED_METHOD(Overloaded, void, float);
	LUA_END_CLASS();

	LUA_BEGIN_SUB_CLASS(Biped, GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(GetC);
	LUA_ADD_METHOD(SetC);
	LUA_END_CLASS();

	LUA_BEGIN_SUB_CLASS(Human, Biped);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_SETTER_GETTER(energy, SetEnergy, GetEnergy);
	// C API
	LUA_ADD_FUNCTION(AddEnergy);
	LUA_ADD_FUNCTION(GetEnergy);
	LUA_END_CLASS();

	LUA_BEGIN_CLASS(Vec3);
	LUA_SET_NEW_OPERATOR_LUA_CFUNC(newVec3);
	LUA_ADD_FUNCTION(add);
	LUA_BOX_OPERATOR();
	LUA_END_CLASS();

	LUA_BEGIN_CLASS(Quat);
	LUA_ADD_FUNCTION(setIdentity);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
