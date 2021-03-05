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
	void overloaded(float) {
	}
	void overloaded(int) {
	}
	std::string getName() const {
		return name;
	}
	const std::string& getNameRef() const {
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

// Lightweight math objects
struct Vec2 {
	float x, y;
};

struct Vec3 {
	double x, y, z;
	inline Vec3 operator + (Vec3 b) const {
		return { x + b.x, y + b.y, z + b.z };
	}
};

struct Quat {
	double x, y, z, w;
};


// Overloading
inline Vec3 cross([[maybe_unused]]Vec3 a, [[maybe_unused]] Vec3 b) {
	return {};
}

inline Vec2 operator - (Vec2 a, Vec2 b) {
	return { a.x + b.x, a.y - b.y };
}

inline Vec3 operator - (Vec3 a, Vec3 b)  {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}
