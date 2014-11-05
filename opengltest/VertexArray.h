//VertexArray.h
#pragma once
#include <gl\glew.h>
#include <glm\glm.hpp>

void AssignSubData(GLuint buffer, void* vertices, unsigned int vertexSize, unsigned int numPoints,
					GLintptr offset);

GLuint CreateSimpleVertexArray(void* vertices, unsigned int numPoints,
								unsigned int* indices, unsigned int numFaces);

GLuint CreateVertexUVArray(void* vertices,unsigned int numPoints,
							unsigned int* indices,unsigned int numFaces);

GLuint CreateVertexNormArray(void* vertices, unsigned int numPoints,
							unsigned int* indices, unsigned int numFaces);

GLuint CreateVertexNormUVArray(void* vertices, unsigned int numPoints,
								unsigned int* indices, unsigned int numFaces);

GLuint CreateBumpmappedVertexArray(void* vertices, unsigned int numPoints,
								unsigned int* indices, unsigned int numFaces);