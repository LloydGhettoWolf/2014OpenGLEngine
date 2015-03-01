//DeferredApp.h
#include "MainApp.h"
#include "GBufferData.h"
#include "DeferredShader.h"
#include "CubemapShader.h"

const int NUM_POINT_LIGHTS = 100;
const int NUM_MESHES = 64;

struct PointLightData{
	vec3  position[NUM_POINT_LIGHTS];
	vec3  color[NUM_POINT_LIGHTS];
	float constantAtt[NUM_POINT_LIGHTS];
	float linearAtt[NUM_POINT_LIGHTS];
	float expAtt[NUM_POINT_LIGHTS];
};

class DeferredApp : public App{
public:

	DeferredApp(){};
	~DeferredApp(){ ShutDown(); }

	bool Init();
	void Run();
	void ShutDown();

private:
	bool InitGUI();

	bool CreateGBuffer();

	void RenderDeferred(const vec3* teapotPositions);
	void RenderGBuffer();
	void RenderLights();

	float CalcSphereDistance(const PointLightData& pLight, int index);

	DeferredShader		  m_deferredShader;
	CubemapShader		  m_cubemapShader;

	StaticMesh            m_teapotMesh,m_sphereMesh, m_cubeMesh;

	GLuint				  m_groundPlaneBuffer,m_cubeMap;

	GBufferData		m_gBuffer;
	PointLightData  m_lights;
	vec3            lightPos[NUM_POINT_LIGHTS];
	vec3            m_positions[NUM_MESHES];
	float           m_radii[NUM_POINT_LIGHTS];

	Material       m_material;

	//full screen quad
	GLuint m_quadBuffer;
};

