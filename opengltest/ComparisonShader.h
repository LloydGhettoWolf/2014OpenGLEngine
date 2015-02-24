//ComparisonShader.h - shader class for the two model, side by side comparison of two Blinn-Phong with refraction/reflection/transparency and directional light
#include "LightsAndMaterials.h"
#include "Shader.h"

struct ComparisonShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint worldMatrixUniform;
	GLint eyePosUniform;
	GLint lightDirUniform;
	GLint normalMatrixUniform;
	GLint cubemapUniform;
	GLint reflectUniform;
	GLint refractUniform;
	GLint solidMixUniform;
	MaterialUniforms matUni;
};

class ComparisonShader{

private:
	static GLuint m_handle;
	ComparisonShaderUniforms m_uniforms;
public:
	ComparisonShader(){};
	bool CreateComparisonShader();
	static GLuint GetHandle(){ return m_handle; }
	const ComparisonShaderUniforms& GetUniforms()const { return m_uniforms; }

	void SetUniforms(const mat4& projMatrix);
	void UpdateUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
		const vec3& pos, const vec3& lightDir);
};