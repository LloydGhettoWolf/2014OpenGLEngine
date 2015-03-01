//DeferredRenderer.h
#include "GBufferData.h"
#include "DeferredShader.h"
#include "StaticMesh.h"

const int NUM_POINT_LIGHTS = 100;

struct PointLightData{
	vec3  position[NUM_POINT_LIGHTS];
	vec3  color[NUM_POINT_LIGHTS];
	float constantAtt[NUM_POINT_LIGHTS];
	float linearAtt[NUM_POINT_LIGHTS];
	float expAtt[NUM_POINT_LIGHTS];
};


class DeferredRenderer{

public:

	DeferredRenderer(){};
	~DeferredRenderer(){};

	void RenderDeferred(const vec3* teapotPositions, void(*RenderFunc)(GLint, mat4&),GLint shaderHandle,mat4& viewProjection,
							 vec3* lightPositions, vec3* lightColors, vec3* camPos);
private:

	bool CreateGBuffer();

	void RenderGBuffer(void(*RenderFunc)(GLint, mat4&),GLint shaderHandle,mat4& viewProjection);
	void RenderLights(mat4& viewProjection, vec3* lightPositions, vec3* lightColors, vec3* camPos);

	float CalcSphereDistance(const PointLightData& pLight, int index);

	DeferredShader		  m_deferredShader;
	StaticMesh            m_sphereMesh;

	GBufferData		m_gBuffer;

	//full screen quad
	GLuint m_quadBuffer;
};

