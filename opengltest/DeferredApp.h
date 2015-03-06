//DeferredApp.h
#include "MainApp.h"
#include "GBufferData.h"
#include "DeferredShader.h"
#include "CubemapShader.h"
#include "DeferredRenderer.h"

const int NUM_POINT_LIGHTS = 100;
const int NUM_MESHES = 64;

class DeferredApp : public App{
public:

	DeferredApp(){};
	~DeferredApp(){ ShutDown(); }

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();

	void RenderDeferred(int numLights);
	void RenderGeometry();
	void RenderCubemap();
	
	DeferredRenderer      m_deferredRenderer;

	PointLightData  m_lights;
	vec3            m_lightPos[NUM_POINT_LIGHTS];
	float           m_radii[NUM_POINT_LIGHTS];

	Material       m_material;

	StaticMesh m_teapotMesh;
	GLuint     m_groundPlaneBuffer;
	vec3       m_positions[NUM_MESHES];
	GLuint	   m_teaTexture, m_normalTexture;

	CubemapShader		  cubemapShader;
	StaticMesh            cubeMesh;
	GLuint				  cubeMap;

	//full screen quad
	GLuint m_quadBuffer;
};


