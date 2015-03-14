//ShadowShader.h
#pragma once
#include "LightsAndMaterials.h"
#include "Shader.h"
#include <gtc\quaternion.hpp>

struct ShadowShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint worldMatrixUniform;
	GLint eyePosUniform;
	GLint lightPosUniform;
	GLint normalMatrixUniform;
	GLint shadowMatrixUniform;
	GLint shadowTextureUniform;
	MaterialUniforms matUni;
};

struct DepthShaderUniforms{
	GLint perspectiveMatrixUniform;
	GLint cameraMatrixUniform;
	GLint worldMatrixUniform;
};

class ShadowShader{

private:
	static GLuint m_shadowHandle, m_depthHandle;
	ShadowShaderUniforms m_shadowUniforms;
	DepthShaderUniforms m_depthUniforms;
public:
	ShadowShader(){};
	~ShadowShader();
	bool CreateShadowShader();

	static GLuint GetShadowHandle(){ return m_shadowHandle; }
	static GLuint GetDepthHandle(){ return m_depthHandle; }

	const ShadowShaderUniforms& GetShadowUniforms()const { return m_shadowUniforms; }
	const DepthShaderUniforms&  GetDepthUniforms()const { return m_depthUniforms;  }

	void SetShadowUniforms(const mat4& projMatrix);
	void SetDepthUniforms(const mat4& projMatrix);

	void UpdateShadowUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
		const vec3& pos, const mat4& shadowMatrix,const vec3& lightPos);
	void UpdateDepthUniforms(const mat4& worldMatrix, const mat4& viewMatrix);
};