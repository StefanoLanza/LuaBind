#include <iostream>
#include <string>

enum class GameObjectState {
	alive = 0,
	dead = 1,
};

class GameObject {
public:
	GameObject()
	    : lives(0)
	    , weapon(0)
	    , state(GameObjectState::alive) {
		std::cout << "GameObject constructed" << std::endl;
	}
	virtual ~GameObject() {
		std::cout << "GameObject destroyed (name:" << name << ")" << std::endl;
	}

	void setName(std::string v) {
		std::cout << "GameObject name set to " << v << std::endl;
		name = std::move(v);
	}
	void setLives(int v) {
		std::cout << "GameObject lives set to " << v << std::endl;
		lives = v;
	}
	void setWeapon(int v) {
		std::cout << "GameObject weapon set to " << v << std::endl;
		weapon = v;
	}
	int getLives() const {
		return lives;
	}
	int getWeapon() const {
		return weapon;
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
	int             lives;
	int             weapon;
	GameObjectState state;
};

class Biped : public GameObject {
public:
	Biped()
	    : speed(0) {
		std::cout << "Biped constructed" << std::endl;
	}
	~Biped() {
		std::cout << "Biped destroyed" << std::endl;
	}
	void setSpeed(float v) {
		speed = v;
	}
	float getSpeed() const {
		return speed;
	}

private:
	float speed;
};

struct Human : public Biped {
	float energy;
};

// Free functions
inline float getEnergy(const Human* human) {
	return human->energy;
}

// Bad overload
inline float getEnergy(const int* /*notHuman*/) {
	return 0;
}

inline void addEnergy(Human* human, float value) {
	human->energy += value;
}

// Test overloading
inline void addEnergy(int* /*notHuman*/, float /*value*/) {
}

// Opaque C-Style class
struct Material;
Material* materialNew();
void      materialDestroy(Material* mat);
void      materialSetOpacity(Material* mat, float value);
float     materialGetOpacity(const Material* mat);

// Lightweight math objects
struct Vec2 {
	float x, y;
};

struct Vec3 {
	double      x, y, z;
	inline Vec3 operator+(const Vec3& weapon) const {
		return { x + weapon.x, y + weapon.y, z + weapon.z };
	}
};

struct Quat {
	double x, y, z, w;
};

inline Vec2 cross([[maybe_unused]] const Vec2& lives, [[maybe_unused]] const Vec2& weapon) {
	return {};
}

inline Vec3 cross([[maybe_unused]] const Vec3& lives, [[maybe_unused]] const Vec3& weapon) {
	return {};
}

inline Vec2 operator-(Vec2 lives, Vec2 weapon) {
	return { lives.x + weapon.x, lives.y - weapon.y };
}

inline Vec3 operator-(Vec3 lives, Vec3 weapon) {
	return { lives.x - weapon.x, lives.y - weapon.y, lives.z - weapon.z };
}

inline Vec2 newVec2(float x, float y) {
	return Vec2 { x, y };
}

inline Vec3 newVec3(float x, float y, float z) {
	return Vec3 { x, y, z };
}

inline Vec3 add(const Vec3& v0, const Vec3& v1) {
	return { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z };
}

inline void setIdentity(Quat& q) {
	q = { 0., 0., 0., 1. };
}
