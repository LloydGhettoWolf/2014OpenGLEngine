//GroundPlane.cpp
#include "GroundPlane.h"
#include "VertexTypes.h"
#include "VertexArray.h"

GLuint CreateGroundPlaneData(){
	int width = 10;
	int length = 10;
	int numVertices = width * length;
	CustomVertexNormUV* groundPlaneData = new CustomVertexNormUV[numVertices];

	int halfWidth = width / 2;
	int halfHeight = length / 2;

	int index = 0;
	//create a buffer of vertex info using the height map
	for (int x = 0; x < width; x++){
		for (int z = 0; z < length; z++){
			index = x * length + z;
			groundPlaneData[index].vertexPoint.x = (x - halfWidth) * 50.0f;
			groundPlaneData[index].vertexPoint.y = -5.0f;
			groundPlaneData[index].vertexPoint.z = (z - halfHeight) * 50.0f;
			groundPlaneData[index].uv.x = x * 0.025f;
			groundPlaneData[index].uv.y = z * 0.025f;
			groundPlaneData[index].normal = vec3(0.0f, 1.0f, 0.0f);
		}
	}

	int numIndices = (width - 1) * (length - 1) * 6;
	unsigned int *indices = new unsigned int[numIndices];

	index = 0;

	for (int x = 0; x < width - 1; x++){
		for (int z = 0; z < length - 1; z++){
			indices[index++] = (width * x) + z;
			indices[index++] = (width * x) + z + 1;
			indices[index++] = (width * (x + 1)) + z;

			indices[index++] = (width * (x + 1)) + z;
			indices[index++] = (width * x) + z + 1;
			indices[index++] = (width * (x + 1)) + z + 1;
		}
	}

	GLuint data = CreateVertexNormUVArray(groundPlaneData, numVertices, indices, numIndices / 3);
	delete[] indices;
	delete[] groundPlaneData;
	return data;
}