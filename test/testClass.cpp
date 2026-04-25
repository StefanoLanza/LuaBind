#include "testClass.h"
#include <algorithm>
#include <cassert>

struct Material {
	char  debug[4];
	float opacity;
};

constexpr const unsigned char debugMarker[4] = { 0xff, 0x12, 0x05, 0x04 };

Material* materialNew(float opacity) {
	auto mat = new Material;
	std::memcpy(mat->debug, debugMarker, sizeof debugMarker);
	mat->opacity = std::clamp(opacity, 0.f, 1.f);
	return mat;
}

void materialDestroy(Material* mat) {
	assert(mat);
	assert(! std::memcmp(mat->debug, debugMarker, sizeof debugMarker));
	delete mat;
}

void materialSetOpacity(Material* mat, float value) {
	mat->opacity = value;
}

float materialGetOpacity(const Material* mat) {
	return mat->opacity;
}
