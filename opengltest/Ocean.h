//Ocean.h - header for the ocean renderer
#pragma once
#include "MainApp.h"
#include "GroundPlane.h"
#include "CubemapShader.h"
#include "ShadowApp.h"

const int NUM_WAVES = 15;

struct WaveData{
	float rho[NUM_WAVES];
	float amplitude[NUM_WAVES];
	vec2  dir[NUM_WAVES];
	float freq[NUM_WAVES];
	float q[NUM_WAVES];
};

class OceanApp : public App
{
public:
	OceanApp(){};
	~OceanApp();

	bool Init();
	void Run();
	void ShutDown();
private:
	void Render();
	bool InitGui();

	bool LoadShader();
	bool CreateWaveTex(GLuint& data, GLuint& waveData);

	Plane			m_groundPlane;

	GLuint m_oceanShader,m_oceanDispshader;

	StaticMesh m_cube;
	GLint m_cubeMap;
	CubemapShader m_cubeMapShader;

	WaveData m_waves;

	TwBar* waveBar;

	//texturestuff
	GLuint m_fbo, m_waveTex;

	//full screen quad
	GLuint m_quadBuffer;

	int m_medianLength = 128, m_medianAmp = 40, m_variation = 90, m_speed = 40;
};