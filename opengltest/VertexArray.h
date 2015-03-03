//VertexArray.h
#pragma once
#include <gl\glew.h>
#include <glm.hpp>

void CreateInstancedAttrib(unsigned int attribNum, GLuint VAO, GLuint& instancedData, int numInstances);

GLuint CreateSimpleVertexArray(void* vertices, unsigned int numPoints,
								unsigned int* indices, unsigned int numFaces);

GLuint CreateVertexArray(void* vertices, unsigned int numPoints, unsigned int vertexSize, unsigned int numComponents,
						unsigned int* indices, unsigned int numFaces);