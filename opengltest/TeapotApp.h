//TeapotApp.h
#pragma once
#include "MainApp.h"

class TeapotApp : public App{
public:
	bool Init();
	void Run();
	void ShutDown();

private:
	void   CreateGroundPlaneData();
	GLuint CreateInstancedLightingShader();
	GLuint CreateLightingShader();

	GLuint		          m_teapotShader;
	GLuint				  m_groundShader;
	StaticMesh            m_teapotMesh;

	GLuint				  m_groundPlaneBuffer;
};