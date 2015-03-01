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

	void RenderDeferred(const vec3* teapotPositions,int numLights);
	static void RenderGeometry(GLint shaderHandle, mat4& viewProjection);
	
	DeferredRenderer      m_deferredRenderer;

	CubemapShader		  m_cubemapShader;
	StaticMesh            m_cubeMesh;
	GLuint				  m_cubeMap;


	GBufferData		m_gBuffer;
	PointLightData  m_lights;
	vec3            lightPos[NUM_POINT_LIGHTS];
	float           m_radii[NUM_POINT_LIGHTS];

	Material       m_material;

	//full screen quad
	GLuint m_quadBuffer;
};


