//sponzaDeferred.h
#pragma once
#include "MainApp.h"
#include "GBufferData.h"
#include "DeferredShader.h"
#include "CubemapShader.h"
#include "DeferredRenderer.h"

const int NUM_POINT_LIGHTS = 100;

class SponzaDeferred : public App{
public:
	SponzaDeferred(){}
	~SponzaDeferred();

	bool Init();
	void Run();
	void ShutDown();

private:

	void RenderDeferred(int numLights);
	static void RenderGeometry(GLint shaderHandle, mat4& viewProjection);
	static void RenderCubemap(mat4& viewProjection, vec3& camPos);

	void SetupUniforms();

	DeferredRenderer      m_deferredRenderer;
	
	//lights
	PointLightData  m_lights;
	vec3            lightPos[NUM_POINT_LIGHTS];
	float           m_radii[NUM_POINT_LIGHTS];

	//full screen quad
	GLuint m_quadBuffer;
};