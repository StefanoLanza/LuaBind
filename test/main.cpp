#include "testClass.h"
#include <core/allocator.h>
#include <core/typedVoidPtr.h>
#include <extras/voidPtrWrapper.h>
#include <luaBind/luaBind.h>

#include <array>
#include <string>
#include <vector>

#define CATCH_CONFIG_RUNNER
#include <Catch-master/single_include/catch2/catch.hpp>

// VISUAL LEAK DETECTOR
#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

void foo() {
	std::cout << "foo" << std::endl;
}
void foo2(const std::string& str) {
	std::cout << "foo2(" << str << ")" << std::endl;
}

// Treat Vec2 as a temporary, lightweight object in Lua
template <>
class LuaBind::Wrapper<Vec2> : public LuaBind::Lightweight<Vec2> {};

// Treat Vec3 as a temporary, lightweight object in Lua
template <>
class LuaBind::Wrapper<Vec3> : public LuaBind::Lightweight<Vec3> {};

void bindTestClasses(lua_State* ls);

int main(int argc, char* argv[]) {
	return Catch::Session().run(argc, argv);
}

TEST_CASE("Root") {
	using namespace LuaBind;

	Typhoon::HeapAllocator heapAllocator;
	lua_State* const       ls = LuaBind::createState(heapAllocator);
	bindTestClasses(ls);

	SECTION("Builtins") {
		constexpr char          c = 127;
		constexpr unsigned char uc = 255;
		constexpr int           i = -1000;
		constexpr unsigned int  ui = 0xFFFF;
		constexpr long          l = -123456789;
		constexpr unsigned long ul = 123456789;
		constexpr float         f = 3.141f;
		constexpr double        d = 2.718281828459045235360287471352662497757247093699951;
		constexpr bool          b = true;
		const char*             cstr = "Stefano";
		const std::string       str { "Lanza" };
		push(ls, c);
		CHECK(pop<char>(ls, -1) == c);
		push(ls, uc);
		CHECK(pop<unsigned char>(ls, -1) == uc);
		push(ls, i);
		CHECK(pop<int>(ls, -1) == i);
		push(ls, ui);
		CHECK(pop<unsigned int>(ls, -1) == ui);
		push(ls, l);
		CHECK(pop<long>(ls, -1) == l);
		push(ls, ul);
		CHECK(pop<unsigned long>(ls, -1) == ul);
		push(ls, f);
		CHECK(pop<float>(ls, -1) == f);
		push(ls, d);
		CHECK(pop<double>(ls, -1) == d);
		push(ls, b);
		CHECK(pop<bool>(ls, -1) == b);
		push(ls, cstr);
		CHECK(pop<std::string>(ls, -1) == cstr);
		push(ls, str);
		CHECK(pop<std::string>(ls, -1) == str);
	}

	SECTION("Warnings") {
		const char* warningMessage = nullptr;
		auto        testWarningLambda = [&warningMessage](const char* message) { warningMessage = message; };
		setWarningFunction(ls, testWarningLambda);
		REQUIRE(doCommand(ls, "warn('This is a warning')"));
		CHECK(! strcmp(warningMessage, "This is a warning"));
	}

	SECTION("Globals") {
		CHECK(doCommand(ls, "Globals.foo()"));
		CHECK(doCommand(ls, "Globals.foo2('hello world')"));
	}

	SECTION("Table") {
		Table table = newTable(ls);

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
			CHECK(static_cast<int>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("uint double") {
			const unsigned int key = 40;
			const double       value = 3.f;
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TNUMBER);
			CHECK(static_cast<double>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("ptr ptr") {
			int         dummy;
			const void* key = &dummy;
			void* const value = &key;
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TLIGHTUSERDATA);
			CHECK(static_cast<void*>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("cstr cstr") {
			const char* key = "key";
			const char* value = "value 0";
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TSTRING);
			CHECK(static_cast<std::string>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("char int") {
			const char key = 'c';
			const int  value = 10;
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TNUMBER);
			CHECK(static_cast<int>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("uchar float") {
			const unsigned char key = 'u';
			const float         value = 3.14f;
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TNUMBER);
			CHECK(static_cast<float>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("table string") {
			const Table       key = newTable(ls);
			const std::string value = "some key";
			table.set(key, value);
			CHECK(lua_gettop(ls) == 0);
			CHECK_FALSE(table[key].isNil());
			CHECK(table[key].getType() == LUA_TSTRING);
			CHECK(static_cast<std::string>(table[key]) == value);
			CHECK(lua_gettop(ls) == 0);
		}
	}

	SECTION("std") {
		SECTION("std::pair") {
			const int idx = lua_gettop(ls);
			using pair_type = std::pair<std::string, int>;
			const pair_type pair { "value", 10 };
			push(ls, pair);
			const pair_type tmp = pop<pair_type>(ls, idx + 1);
			CHECK(pair == tmp);
		}

		SECTION("std::vector") {
			const int idx = lua_gettop(ls);
			using vec_type = std::vector<std::string>;
			const vec_type vec1 { "stefano", "claudio", "cristiana", "manlio", "nicoletta" };
			push(ls, vec1);
			const vec_type vec2 = pop<vec_type>(ls, idx + 1);
			CHECK(vec1 == vec2);
		}

		SECTION("std::array") {
			using arrayType = std::array<std::string, 5>;
			const arrayType stringArray = { "stefano", "claudio", "cristiana", "nico", "manlio" };
			const int       idx = lua_gettop(ls);
			push(ls, stringArray);
			const arrayType testArray = pop<arrayType>(ls, idx + 1);
			CHECK(stringArray == testArray);
		}
	}

	SECTION("Properties") {
		Table globals = getGlobals(ls);
		if (globals["ptest"]) {
			Table ptest = (Table)globals["ptest"];
			REQUIRE(ptest);

			Table properties = (Table)ptest["p"];
			REQUIRE(properties);
		}
	}

	SECTION("VoidPtr") {
		Biped biped;
		biped.SetName("Biped");
		const Typhoon::VoidPtr voidPtr = Typhoon::MakeVoidPtr(&biped);
		push(ls, voidPtr);
	}

	SECTION("Class") {
		Table globals = getGlobals(ls);
		Table registry = getRegistry(ls);
		SECTION("base class") {
			SECTION("binding C++ object as full user data") {
				auto       bart = std::make_unique<GameObject>();
				const auto ref = registerObjectAsUserData(ls, bart.get());
				REQUIRE(ref.isValid());
				CHECK(registry[bart.get()].isNil() == false);
				globals.set("bart", bart.get());
				CHECK(globals["bart"].getType() == LUA_TUSERDATA);
				CHECK(doCommand(ls, "bart:SetA(10)"));
				CHECK(bart->GetA() == 10);
				CHECK(doCommand(ls, "bart:SetName('Bart')"));
				CHECK(bart->getName() == "Bart");
				if (ref) {
					unregisterObject(ls, ref);
				}
				CHECK(registry[bart.get()].isNil());
				globals.set("bart", nil);
				CHECK(lua_gettop(ls) == 0);
			}

			SECTION("binding C++ object as table") {
				auto       fred = std::make_unique<GameObject>();
				const auto ref = registerObjectAsTable(ls, fred.get());
				REQUIRE(ref.isValid());
				CHECK(registry[fred.get()].isNil() == false);
				globals.set("fred", fred.get());
				CHECK(globals["fred"].getType() == LUA_TTABLE);
				CHECK(doCommand(ls, "fred:SetA(20)"));
				CHECK(fred->GetA() == 20);
				CHECK(doCommand(ls, "fred:SetName('Fred')"));
				CHECK(fred->getName() == "Fred");
				unregisterObject(ls, ref);
				CHECK(registry[fred.get()].isNil() == true);
				globals.set("fred", nil);
				CHECK(lua_gettop(ls) == 0);
			}

			SECTION("binding C++ object as light user data") {
				auto       barney = std::make_unique<GameObject>();
				const auto ref = registerObjectAsLightUserData(ls, barney.get());
				REQUIRE(ref.isValid());
				globals.set("barney", barney.get());
				CHECK(globals["barney"].getType() == LUA_TLIGHTUSERDATA);

				CHECK(doCommand(ls, "GameObject.SetA(barney, 20)"));
				CHECK(barney->GetA() == 20);
				CHECK(doCommand(ls, "GameObject.SetName(barney, 'Barney')"));
				CHECK(barney->getName() == "Barney");

				unregisterObject(ls, ref);
				globals.set("barney", nil);
				CHECK(lua_gettop(ls) == 0);
			}

			SECTION("creating C++ object in Lua") {
				CHECK(doCommand(ls, "obj = GameObject.new()"));
				CHECK(doCommand(ls, "obj:SetA(20)"));
				CHECK(doCommand(ls, "obj:SetName('Stanley')"));
				CHECK(doCommand(ls, "obj:setState(0)"));
				CHECK(doCommand(ls, "obj:getState()"));
				CHECK(doCommand(ls, "obj = nil"));
				CHECK(lua_gettop(ls) == 0);
			}
		}

		SECTION("sub class") {
			auto       biped = std::make_unique<Biped>();
			const auto ref = registerObjectAsUserData(ls, biped.get());
			REQUIRE(ref.isValid());
			globals.set("subobj", ref);
			const Biped* tmp2 = static_cast<const Biped*>(globals["subobj"]);
			REQUIRE(tmp2);
			CHECK(globals["subobj"].getType() == LUA_TUSERDATA);
			doCommand(ls, "subobj:SetA(20)");
			CHECK(biped->GetA() == 20);
			doCommand(ls, "subobj:SetC(30)");
			CHECK(biped->GetC() == 30.f);
			doCommand(ls, "subobj:SetName('Homer')");
			CHECK(biped->getNameRef() == "Homer");

			unregisterObject(ls, ref);
			globals.set("subobj", nil);
			CHECK(lua_gettop(ls) == 0);
		}

		SECTION("struct") {
			Human human;
			human.energy = 10;
			const auto ref = registerObjectAsUserData(ls, &human);
			REQUIRE(ref.isValid());

			globals.set("human", ref);

			CHECK_FALSE(globals["human"].isNil());

			CHECK(doCommand(ls, "human:SetName('Rocky')"));
			CHECK(human.getName() == "Rocky");

			doCommand(ls, "energy = human:GetEnergy()");
			const float energy = (float)globals["energy"];
			CHECK(energy == human.energy);

			doCommand(ls, "energy = human:SetEnergy(20)");
			CHECK(20 == human.energy);

			// C API
			doCommand(ls, "Human.AddEnergy(human, 10.)");
		}

		SECTION("temporaries") {
			CHECK(doCommand(ls, "vec0 = Vec3.new(0., 1., 2.)"));
			CHECK(doCommand(ls, "vec1 = Vec3.new(3., 4., 5.)"));
			CHECK(doCommand(ls, "vec2 = Vec3.add(vec0, vec1)"));
			const Vec3* vptr = static_cast<const Vec3*>(globals["vec0"]);
			REQUIRE(vptr);
			if (vptr) {
				CHECK(vptr->x == 0.);
				CHECK(vptr->y == 1.);
				CHECK(vptr->z == 2.);
			}
			const Vec3* v1ptr = static_cast<const Vec3*>(globals["vec1"]);
			REQUIRE(v1ptr);
			if (v1ptr) {
				CHECK(v1ptr->x == 3.);
				CHECK(v1ptr->y == 4.);
				CHECK(v1ptr->z == 5.);
			}
			const Vec3* v2ptr = static_cast<const Vec3*>(globals["vec2"]);
			REQUIRE(v2ptr);
			if (v2ptr) {
				CHECK(v2ptr->x == 3.);
				CHECK(v2ptr->y == 5.);
				CHECK(v2ptr->z == 7.);
			}
#if TY_LUABIND_TYPE_SAFE
			const Vec3* v = static_cast<const Vec3*>(globals["vec0"]);
			CHECK(v);
			CHECK(! doCommand(ls, "Quat.setIdentity(vec0)"));
			const Quat* q = static_cast<const Quat*>(globals["vec0"]);
			CHECK(q == nullptr);
#endif
		}
	}

	SECTION("UniqueRef") {
		Table registry = getRegistry(ls);
		auto  obj = std::make_unique<GameObject>();
		obj->SetName("UniqueRef");
		const auto ref = registerObjectAsUserData(ls, obj.get());
		CHECK(registry[ref].getType() == LUA_TUSERDATA);

		SECTION("destruction") {
			{
				UniqueRef uref = makeUniqueRef(ls, ref);
				REQUIRE(uref.get());
				REQUIRE(static_cast<bool>(uref));
			}
			CHECK(registry[ref].getType() != LUA_TUSERDATA);
		}
		SECTION("release") {
			UniqueRef uref = makeUniqueRef(ls, ref);
			uref.release();
			REQUIRE(! uref.get());
			REQUIRE(static_cast<bool>(uref) == false);
			CHECK(registry[ref].getType() == LUA_TUSERDATA);
		}
		SECTION("reset") {
			UniqueRef uref = makeUniqueRef(ls, ref);
			uref.reset();
			REQUIRE(! uref.get());
			REQUIRE(static_cast<bool>(uref) == false);
			CHECK(registry[ref].getType() != LUA_TUSERDATA);
		}
		SECTION("move") {
			UniqueRef uref = makeUniqueRef(ls, ref);
			REQUIRE(uref.get());
			UniqueRef uref2 = std::move(uref);
			REQUIRE(! uref.get());
			REQUIRE(uref2.get());
			CHECK(registry[ref].getType() == LUA_TUSERDATA);
		}
	}

	SECTION("Opaque class from C++") {
		auto       mat = MaterialNew();
		const auto ref = registerObjectAsUserData(ls, mat);
		auto       registry = getRegistry(ls);
		CHECK(registry[ref].getType() == LUA_TUSERDATA);
		MaterialDestroy(mat);
	}

	// FIXME Destructor not called , mat not collected
	SECTION("Opaque class from Lua") {
		CHECK(doCommand(ls, R"(
			local mat = Material.new()
			Material.setOpacity(mat, 0.5)
		)"));
	}

	LuaBind::closeState(ls);
}

void bindTestClasses(lua_State* ls) {
	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_NAMESPACE(Globals);
	LUA_ADD_FREE_FUNCTION(foo);
	LUA_ADD_FREE_FUNCTION(foo2);
	LUA_END_NAMESPACE();

	LUA_BEGIN_CLASS(GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(SetName);
	LUA_ADD_METHOD(getNameRef);
	LUA_ADD_METHOD(getName);
	LUA_ADD_METHOD(SetA);
	LUA_ADD_METHOD(SetB);
	LUA_ADD_METHOD(GetA);
	LUA_ADD_METHOD(GetB);
	LUA_ADD_METHOD(getState);
	LUA_ADD_METHOD(setState);
	LUA_ADD_OVERLOADED_METHOD(overloaded, void, float);
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

	LUA_BEGIN_CLASS(Vec2);
	LUA_NEW_OPERATOR(newVec2);
	LUA_ADD_FUNCTION(cross);
	LUA_ADD_FREE_OPERATOR(sub, -);
	LUA_END_CLASS();

	LUA_BEGIN_CLASS(Vec3);
	LUA_NEW_OPERATOR(newVec3);
	LUA_ADD_FUNCTION(add);
	LUA_ADD_FUNCTION(cross);
	LUA_ADD_OPERATOR(add, +);
	LUA_ADD_FREE_OPERATOR(sub, -);
	LUA_END_CLASS();

	LUA_BEGIN_CLASS(Quat);
	LUA_ADD_FUNCTION(setIdentity);
	LUA_END_CLASS();

	LUA_BEGIN_CLASS(Material);
	LUA_NEW_OPERATOR(MaterialNew);
	LUA_DELETE_OPERATOR(MaterialDestroy);
	LUA_FUNCTION_RENAMED(MaterialSetOpacity, setOpacity);
	LUA_FUNCTION_RENAMED(MaterialGetOpacity, getOpacity);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
