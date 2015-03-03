//DeferredRenderer.h
#pragma once
#include "GBufferData.h"
#include "DeferredShader.h"
#include "StaticMesh.h"
#include "LightsAndMaterials.h"

class DeferredRenderer{

public:

	DeferredRenderer(){};
	~DeferredRenderer(){};

	bool Init();

	void SetUniformsFirstTime(vec2& screenSize,mat3& normalMatrix,mat4& rotation);

	void RenderDeferred(const vec3* teapotPositions,mat4& viewProjection, vec3* lightPositions, vec3* lightColors, 
						vec3& camPos, int numLights, void(*RenderFunc)(GLint, mat4&), void(*CubeMapFunc)(mat4&,vec3&) = NULL);
private:

	bool CreateGBuffer();

	void RenderGBuffer(void(*RenderFunc)(GLint, mat4&),mat4& viewProjection);
	void RenderLights(mat4& viewProjection, vec3* lightPositions, vec3* lightColors, vec3& camPos,int numLights);

	float CalcSphereDistance(const PointLightData& pLight, int index);

	DeferredShader		  m_deferredShader;
	StaticMesh            m_sphereMesh;

	GBufferData			  m_gBuffer;

	//full screen quad
	GLuint m_quadBuffer;
};

