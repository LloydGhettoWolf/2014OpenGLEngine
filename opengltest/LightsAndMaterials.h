//LightsAndMaterials.h
#pragma once
#include <gl\glew.h>
#include <glm.hpp>
#include "Defines.h"
using namespace glm;


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

struct PointLightData{
	vec3  *position;
	vec3  *color;
	float *constantAtt;
	float *linearAtt;
	float *expAtt;

	~PointLightData(){
		delete [] position;
		delete [] color;
		delete [] constantAtt;
		delete [] linearAtt;
		delete [] expAtt;
	}
};

