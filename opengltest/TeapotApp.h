//TeapotApp.h
#pragma once
#include "MainApp.h"
#include "ForwardShader.h"
#include "CubemapShader.h"
#include "PointSprite.h"
#include "GroundPlane.h"

const int NUM_POINT_LIGHTS = 100;


class TeapotApp : public App{
public:

	TeapotApp(){};
	~TeapotApp(){ ShutDown(); }

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();


	void RenderForward(const vec3* lightPositions, const vec3* teapotPositions);


	ForwardShader		  m_teapotShader;
	CubemapShader		  m_cubemapShader;

	StaticMesh            m_teapotMesh,m_cubeMesh;

	GLuint				  m_cubeMap;

	Plane			m_groundPlane;

	PointLightData  m_lights;
	vec3            lightPos[100];
	float           m_radii[100];

	Material       m_material;

	TwBar* myBar;
};

