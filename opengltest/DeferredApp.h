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
	static void RenderGeometry(GLint shaderHandle, mat4& viewProjection);
	static void RenderCubemap(mat4& viewProjection, vec3& camPos);
	
	DeferredRenderer      m_deferredRenderer;

	PointLightData  m_lights;
	vec3            lightPos[NUM_POINT_LIGHTS];
	float           m_radii[NUM_POINT_LIGHTS];

	Material       m_material;

	//full screen quad
	GLuint m_quadBuffer;
};


