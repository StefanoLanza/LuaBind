#include "testClass.h"
#include <algorithm>

struct Material {
	float opacity;
};

Material* materialNew(float opacity) {
	auto mat = new Material;
	mat->opacity = std::clamp(opacity, 0.f, 1.f);
	return mat;
}

void materialDestroy(Material* mat) {
	delete mat;
}

void materialSetOpacity(Material* mat, float value) {
	mat->opacity = value;
}

float materialGetOpacity(const Material* mat) {
	return mat->opacity;
}
