//GroundPlane.cpp
#include "GroundPlane.h"
#include "VertexTypes.h"
#include "VertexArray.h"

void  CreatePlaneData(Plane& planeData, int width, int length, float size, float textureDiv){
	int numVertices = width * length;
	CustomVertexNormBiTangentUV* groundPlaneData = new CustomVertexNormBiTangentUV[numVertices];

	int halfWidth = width / 2;
	int halfHeight = length / 2;

	int index = 0;
	//create a buffer of vertex info using the height map
	for (int x = 0; x < width; x++){
		for (int z = 0; z < length; z++){
			index = x * length + z;
			groundPlaneData[index].vertexPoint.x = (x - width/2)  * size;
			groundPlaneData[index].vertexPoint.y = -5.0f;
			groundPlaneData[index].vertexPoint.z = (z - length/2) * size;
			groundPlaneData[index].normal	 = vec3(0.0f, 1.0f, 0.0f);
			groundPlaneData[index].biTangent = vec3(1.0f, 0.0f, 0.0f);
			groundPlaneData[index].tangent   = vec3(0.0f, 0.0f, -1.0f);
			groundPlaneData[index].uv.x = x * textureDiv;
			groundPlaneData[index].uv.y = z * textureDiv;
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

	GLuint data = CreateVertexArray(groundPlaneData, numVertices, sizeof(CustomVertexNormBiTangentUV), 5, indices, numIndices / 3);
	
	delete[] indices;
	delete[] groundPlaneData;
	planeData.planeBuffer = data;
}