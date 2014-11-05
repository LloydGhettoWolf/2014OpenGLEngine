//VertexTypes.h
#pragma once
#include <glm\glm.hpp>
using namespace glm;

struct CustomVertexUV{
	glm::vec3 vertexPoint;
	glm::vec2 uv;
};

struct CustomVertexNormUV{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct CustomVertexNorm{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
};

struct CustomVertexNormBiNormUV{
	glm::vec3 vertexPoint;
	glm::vec3 normal;
	glm::vec2 uv;
};
