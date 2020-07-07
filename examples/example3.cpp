// This example shows how to access interact with objects defined in Lua

#include <include/luaBind.h>
#include <iostream>
#include <string>

void bindClass(lua_State* ls);
void example(lua_State* ls);

enum class GameObjectState {
	alive = 0,
	dead = 1,
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

private:
	std::string     name;
	GameObjectState state;
};

class Monster : public GameObject {
public:
	Monster()
	    : hunger(0) {
		std::cout << "Monster::constructor" << std::endl;
	}
	~Monster() {
		std::cout << "Monster::destructor (name: " << getName() << ")" << std::endl;
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
	local luaObj = GameObject()
	luaObj:setName("lua-owned")
	local name = luaObj:getName()
	print ("luaObj name:"..name)

	-- Objects created by native code and bound to Lua

	-- cppObj1 is a user data
	print(type(cppObj0))
	local name = cppObj0:getName()
	print ("cppObj0 name:"..name)

	-- cppObj1 is a table
	print(type(cppObj1))
	local name = cppObj1:getName()
	print ("cppObj1 name:"..name)

	-- We can store custom fields in an object represented by a table
	cppObj1.customField = "someCustomField"

	-- Derived game object
	local name = cppMonsterObj0:getName()
	print ("cppMonsterObj0 name:"..name)
)";

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	lua_State* const ls = LuaBind::createState(8192);
	bindClass(ls);
	example(ls);
	LuaBind::closeState(ls);

	return 0;
}

void example(lua_State* ls) {
	using namespace LuaBind;

	// Construct objects
	GameObject obj0;
	obj0.setName("cppObj0");

	GameObject obj1;
	obj1.setName("cppObj1");

	Monster monster0;
	monster0.setName("cppMonster0");

	// Expose cpp object to Lua as a full userdata
	const Reference ref0 = registerObjectAsUserData(ls, &obj0);
	globals(ls).rawSet("cppObj0", ref0);

	// Expose cpp object to Lua as a table. This way, in Lua we can associate custom elements to the object
	const Reference ref1 = registerObjectAsTable(ls, &obj1);
	globals(ls).rawSet("cppObj1", ref1);

	const Reference ref2 = registerObjectAsUserData(ls, &monster0);
	globals(ls).rawSet("cppMonsterObj0", ref2);

	if (Result res = doCommand(ls, script); ! res) {
		std::cout << res.getErrorMessage() << std::endl;
		return;
	}
}

void bindClass(lua_State* ls) {
	using namespace LuaBind;

	LUA_BEGIN_BINDING(ls);

	LUA_BEGIN_CLASS(GameObject);
	LUA_SET_DEFAULT_NEW_OPERATOR();
	LUA_ADD_METHOD(setName);
	LUA_ADD_METHOD(getName);
	LUA_ADD_METHOD(getState);
	LUA_ADD_METHOD(setState);
	LUA_END_CLASS();

	LUA_BEGIN_SUB_CLASS(Monster, GameObject);
	LUA_ADD_METHOD(setHunger);
	LUA_ADD_METHOD(getHunger);
	LUA_END_CLASS();

	LUA_END_BINDING();
}
