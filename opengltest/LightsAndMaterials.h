//LightsAndMaterials.h
#pragma once
#include <gl\glew.h>
#include <glm\glm.hpp>
#include "Defines.h"
using namespace glm;

struct PointLightData{
	vec3  position[NUM_POINT_LIGHTS];
	vec3  color[NUM_POINT_LIGHTS];
	float constantAtt[NUM_POINT_LIGHTS];
	float linearAtt[NUM_POINT_LIGHTS];
	float expAtt[NUM_POINT_LIGHTS];
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

struct MaterialUniforms{
	GLuint diffuseUniform;
	GLuint ambientUniform;
	GLuint specularUniform;
	GLuint shininessUniform;
};
