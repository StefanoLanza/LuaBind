// This example shows how to register C++ objects and access them in Lua

#include <include/luaBind.h>
#include <include/version.h>

#include <core/allocator.h>
#include <iostream>
#include <string>

void bind(lua_State* ls);
void runExample(lua_State* ls);

enum class GameObjectState {
	alive = 0,
	dead = 1,
};

enum class Weapon {
	hand = 0,
	rifle = 1,
};

class GameObject {
public:
	GameObject()
	    : state(GameObjectState::alive) {
		std::cout << "GameObject::constructor" << std::endl;
	}
	virtual ~GameObject() {
		std::cout << "GameObject::destructor (name: " << name << ")" << std::endl;
	}

	void setName(std::string v) {
		std::cout << "GameObject::setName (arg: " << v << ")" << std::endl;
		name = std::move(v);
	}
	const std::string& getName() const {
		std::cout << "GameObject::getName" << std::endl;
		return name;
	}
	GameObjectState getState() const {
		std::cout << "GameObject::getState" << std::endl;
		return state;
	}
	void setState(GameObjectState s) {
		std::cout << "GameObject::setState" << std::endl;
		state = s;
	}

protected:
	std::string     name;
	GameObjectState state;
};

class Human : public GameObject {
public:
	Human()
	    : weapon(Weapon::hand) {
		std::cout << "Human::constructor" << std::endl;
	}
	~Human() {
		std::cout << "Human::destructor (name: " << name << ")" << std::endl;
	}
	Weapon getWeapon() const {
		return weapon;
	}
	void setWeapon(Weapon newValue) {
		weapon = newValue;
	}

private:
	Weapon weapon;
};

class Monster : public GameObject {
public:
	Monster()
	    : hunger(0) {
		std::cout << "Monster::constructor" << std::endl;
	}
	~Monster() {
		std::cout << "Monster::destructor (name: " << name << ")" << std::endl;
	}
	float getHunger() const {
		return hunger;
	}
	void setHunger(float newValue) {
		hunger = newValue;
	}

private:
	float hunger;
};

const char* script = R"(
	-- Create an object in Lua directly
--	local luaHuman = Human.new()
--	luaHuman:setName("luaHuman")
--	local name = luaHuman:getName()
--	print ("luaHuman name:"..name)

	-- cppHuman and cppMonster were created by native code and bound to Lua

	-- cppMonster is a user data
	assert(type(cppHuman) == "userdata")
	local name = cppHuman:getName()
	print ("cppHuman name:"..name)

	-- cppMonster is a table
	assert(type(cppMonster) == "table")
	local name = cppMonster:getName()
	print ("cppMonster name:"..name)
	cppMonster:setHunger(100)
	local hunger = cppMonster:getHunger()
	print ("cppMonster hunger:"..hunger)

	-- We can store custom fields in an object represented by a table. Useful for gameplay
	cppMonster.preys = { cppHuman, luaHuman }
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

	// Construct objects
	Human obj0;
	obj0.setName("cppHuman");

	Monster obj1;
	obj1.setName("cppMonster");

	// Expose cpp object to Lua as a full userdata
	const Reference ref0 { registerObjectAsUserData(ls, &obj0)/*, ls*/ };
	globals(ls).rawSet("cppHuman", ref0);

	// Expose cpp object to Lua as a table. This way, in Lua we can associate custom elements to the object
	const Reference ref1 = registerObjectAsTable(ls, &obj1);
	globals(ls).rawSet("cppMonster", ref1);

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
	}

	// Unregister objects
	unregisterObject(ls, ref0);
	unregisterObject(ls, ref1);
}

void bind(lua_State* ls) {
	using namespace LuaBind;

	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_CLASS(GameObject);
	LUA_ADD_METHOD(setName);
	LUA_ADD_METHOD(getName);
	LUA_ADD_METHOD(getState);
	LUA_ADD_METHOD(setState);
	LUA_END_CLASS();

	LUA_BEGIN_SUB_CLASS(Human, GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(setWeapon);
	LUA_ADD_METHOD(getWeapon);
	LUA_END_CLASS();

	LUA_BEGIN_SUB_CLASS(Monster, GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(setHunger);
	LUA_ADD_METHOD(getHunger);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
