//DeferredRenderer.h
#pragma once
#include "GBufferData.h"
#include "DeferredShader.h"
#include "StaticMesh.h"
#include "LightsAndMaterials.h"

class DeferredRenderer{

public:

	DeferredRenderer(){};
	~DeferredRenderer(){ glDeleteBuffers(1, &m_quadBuffer); }

	bool Init();

	void SetUniformsFirstTime(vec2& screenSize,mat3& normalMatrix,mat4& rotation);

	void PresentToScreen();

	void DeferredRenderer::BindFBO();
	void DeferredRenderer::UnbindFBO();
	void DeferredRenderer::PrepareGeometry();

	GLint GetGBufferShaderHandle()const { return m_deferredShader.GetGBufferHandle(); }
	void RenderLights(mat4& viewProjection, vec3* lightPositions, vec3* lightColors, vec3& camPos, int numLights);
private:

	bool CreateGBuffer();
	float CalcSphereDistance(const PointLightData& pLight, int index);

	DeferredShader		  m_deferredShader;
	StaticMesh            m_sphereMesh;

	GBufferData			  m_gBuffer;

	//full screen quad
	GLuint m_quadBuffer;
};

