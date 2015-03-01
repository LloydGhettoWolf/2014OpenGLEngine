//ComparisonApp.h - A comparison app for blinn-phong vs BDRF lighting models. 

//TeapotApp.h
#pragma once
#include "MainApp.h"
#include "ComparisonShader.h"
#include "CookTorrance.h"
#include "CubemapShader.h"
#include <gtc\quaternion.hpp>

const int NUM_POINT_LIGHTS = 100;


class ComparisonApp : public App{
public:

	ComparisonApp(){};
	~ComparisonApp(){ ShutDown();}

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();

	void Render();

	ComparisonShader	  m_modelShader;
	CubemapShader		  m_cubemapShader;
	CookTorranceShader	  m_ctShader;

	StaticMesh            m_exampleMesh,m_cubeMesh;

	GLuint				  m_cubeMap;

	PointLightData  m_lights;
	vec3            m_lightDir;

	Material        m_material1;

	
	TwBar* BlinnGUI;
	TwBar* BDRFGUI;
	TwBar* LightDir;

	quat m_model1Rotation, m_model2Rotation;
	float m_centerOffset, m_reflect1,refractIndex1,solidMix1;

	//cook torrance 
	float roughness, fresnel, k,refractiveIndex,n;
};

