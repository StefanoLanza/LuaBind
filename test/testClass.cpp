#include "testClass.h"

struct Material {
	float opacity;
};

Material* materialNew() {
	auto mat = new Material;
	mat->opacity = 1.f;
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
