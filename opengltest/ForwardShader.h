//ForwardShader.h 
#pragma once
#include "LightsAndMaterials.h"
#include "Shader.h"

struct ForwardShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint textureUniform;
	GLint worldMatrixUniform;
	GLint eyePosUniform;
	GLint lightVecUniform;
	GLint lightColUniform;
	GLint normalMatrixUniform;
	MaterialUniforms matUni;
	GLint instancedUniform;
};

class ForwardShader{

private:
	static GLuint m_handle;
	ForwardShaderUniforms m_uniforms;
public:
	ForwardShader(){};
	bool CreateForwardShader(bool withUVs = true);
	static GLuint GetHandle(){ return m_handle; }
	const ForwardShaderUniforms& GetUniforms()const { return m_uniforms; }

	void SetUniforms(const mat4& projMatrix, const vec3& lightColors);
	void UpdateUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
						const vec3& pos, const vec3& lightPos, int instanced);
};