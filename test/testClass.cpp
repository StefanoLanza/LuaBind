#include "testClass.h"

struct Material {
	float opacity;
};

Material* MaterialNew() {
	auto mat = new Material;
	mat->opacity = 1.f;
	return mat;
}

void MaterialDestroy(Material* mat) {
	delete mat;
}

void MaterialSetOpacity(Material* mat, float value) {
	mat->opacity = value;
}

float MaterialGetOpacity(const Material* mat) {
	return mat->opacity;
}
