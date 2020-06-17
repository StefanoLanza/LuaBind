#include <iostream>
#include <memory>
#include <string>

enum class GameObjectState {
	alive = 0,
	dead = 1,
};

class GameObject {
public:
	GameObject()
	    : a(0)
	    , b(0)
	    , state(GameObjectState::alive) {
		std::cout << "Avatar created" << std::endl;
	}
	virtual ~GameObject() {
		std::cout << "Avatar deleted (name:" << name << ")" << std::endl;
	}

	void SetName(std::string v) {
		std::cout << "Name set to " << v << std::endl;
		name = std::move(v);
	}
	void SetA(int v) {
		std::cout << "SetA " << v << std::endl;
		a = v;
	}
	void SetB(int v) {
		b = v;
	}
	int GetA() const {
		return a;
	}
	int GetB() const {
		return b;
	}
	GameObjectState getState() const {
		return state;
	}
	void setState(GameObjectState s) {
		state = s;
	}
	void Overloaded(float) {
	}
	void Overloaded(int) {
	}
	std::string GetName() const {
		return name;
	}
	const std::string& GetNameRef() const {
		return name;
	}

private:
	std::string     name;
	int             a;
	int             b;
	GameObjectState state;
};

class Biped : public GameObject {
public:
	Biped()
	    : c(0) {
		std::cout << "Biped created" << std::endl;
	}
	~Biped() {
		std::cout << "Biped deleted" << std::endl;
	}
	void SetC(float v) {
		c = v;
	}
	float GetC() const {
		return c;
	}

private:
	float c;
};

struct Human : public Biped {
	float energy;
};

// Free functions
inline float GetEnergy(const Human* human) {
	return human->energy;
}

// Bad overload
inline float GetEnergy(const int* /*notHuman*/) {
	return 0;
}

inline void AddEnergy(Human* human, float value) {
	human->energy += value;
}

// Test overloading
inline void AddEnergy(int* /*notHuman*/, float /*value*/) {
}
