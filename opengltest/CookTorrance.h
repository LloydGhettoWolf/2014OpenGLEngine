//CookTorrance.h
#include "LightsAndMaterials.h"
#include "Shader.h"

struct CookTorranceShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint worldMatrixUniform;
	GLint eyePosUniform;
	GLint lightDirUniform;
	GLint normalMatrixUniform;
	
	GLint roughnessUniform;
	GLint fresnelUniform;
	GLint kUniform;
};

class CookTorranceShader{

private:
	static GLuint m_handle;
	CookTorranceShaderUniforms m_uniforms;
public:
	CookTorranceShader(){};
	bool CreateCookTorranceShader();
	static GLuint GetHandle(){ return m_handle; }
	const CookTorranceShaderUniforms& GetUniforms()const { return m_uniforms; }

	void SetUniforms(const mat4& projMatrix);
	void UpdateUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
		const vec3& pos, const vec3& lightDir);
};