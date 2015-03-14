//groundplane.h - to store the ground plane visible in the demo
#pragma once
#include <gl\glew.h>

struct Plane{
	GLuint planeBuffer;

	~Plane(){ glDeleteVertexArrays(1, &planeBuffer); }
};

void  CreatePlaneData(Plane& planeData,int width, int length, float size, float textureDiv);