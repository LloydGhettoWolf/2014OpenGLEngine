//SponzaApp.h
#pragma once
#include "MainApp.h"

class SponzaApp: public App{
public:
	bool Init();
	void Run();
	void ShutDown();

private:
	GLuint CreateLightingShader();

	GLuint		          m_lightingShader;
	GLuint                m_depthBuffer;
	StaticMesh            m_sponzaMesh;
}; 