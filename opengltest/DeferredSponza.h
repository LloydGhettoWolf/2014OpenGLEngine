//DeferredSponza - deferred version of my sponza render
#include "MainApp.h"
#include "GBufferData.h"
#include "DeferredShader.h"
#include "CubemapShader.h"
#include "DeferredRenderer.h"

const int NUM_POINT_LIGHTS = 600;

class DeferredSponza : public App{
public:

	DeferredSponza(){};
	~DeferredSponza(){ ShutDown(); }

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();

	void RenderDeferred(int numLights);
	void RenderSponza();
	void RenderCubemap();

	DeferredRenderer      m_deferredRenderer;

	PointLightData  m_lights;
	vec3            m_lightPos[NUM_POINT_LIGHTS];
	float           m_radii[NUM_POINT_LIGHTS];

	Material       m_material;

	StaticMesh		m_sponzaMesh;

	CubemapShader		  cubemapShader;
	StaticMesh            cubeMesh;
	GLuint				  cubeMap;

	string			m_fps;
	TwBar*          FPSGUI;
	int             m_numVisibleLights = 0;
	int             m_numVisibleMeshes = 0;

	//full screen quad
	GLuint m_quadBuffer;
};


