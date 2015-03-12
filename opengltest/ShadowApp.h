//ShadowApp.h - application for simple shadows 
#pragma once
#include "MainApp.h"
#include "ShadowShader.h"
#include "DepthBuffer.h"
#include "GroundPlane.h"

class ShadowApp : public App{
public:

	ShadowApp(){};
	~ShadowApp(){ ShutDown(); }

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();
	void RenderShadow();
	void RenderMeshes();

	ShadowShader	  m_shadowShader;

	StaticMesh        m_teapotMesh;
	GLuint			  m_groundPlane;

	float m_centerOffset;

	vec3            m_lightPos,m_lightDir;
	Material        m_material;
	Camera          m_shadowCamera;

	TwBar* MaterialGUI;
	TwBar* ShadowGUI;
	TwBar* LightDir;

	mat4 m_worldMatrix,m_shadowMatrix,m_biasMatrix,m_scaleMatrix;
	mat3 m_normalMatrix;

	quat m_model1Rotation;

	GLuint m_fbo,m_depthTexture; 

	float m_bias;
};

