//SponzaApp.h
#pragma once
#include "MainApp.h"
#include "CubemapShader.h"
#include "ShadowShader.h"

class SponzaApp: public App{
public:
	bool Init();
	void Run();
	void ShutDown();

private:
	GLuint CreateLightingShader();
	void RenderShadow();

	//sponza mesh
	GLuint									 m_lightingShader;
	StaticMesh					             m_sponzaMesh;

	//cube map
	StaticMesh            cubeMesh;
	GLuint				  cubeMap;
	CubemapShader		  cubemapShader;

	//shadow shader
	ShadowShader	  m_shadowShader;
	GLuint			  m_fbo, m_depthTexture;
	Camera            m_shadowCamera;

	vec3              m_lightPos;
}; 