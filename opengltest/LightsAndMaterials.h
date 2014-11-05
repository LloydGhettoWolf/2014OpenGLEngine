//LightsAndMaterials.h
#pragma once
#include <glm\glm.hpp>
using namespace glm;

struct PointLight{
	vec3 position;
	vec3 color;
	float constantAtt;
	float linearAtt;
	float expAtt;
};

struct DirectionalLight{
	vec3 direction;
	vec3 color;
};

struct SpotLight{
	vec3 pos;
	vec3 direction;
	float cutoff;
};

struct Material{
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
	float shininess;
};