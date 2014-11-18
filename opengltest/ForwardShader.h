//ForwardShader.h 
#pragma once
#include "LightsAndMaterials.h"
#include "Shader.h"

struct ForwardShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint textureUniform;
	GLint rotationUniform;
	GLint eyePosUniform;
	GLint lightVecUniform;
	GLint lightColUniform;
	GLint normalMatrixUniform;
	MaterialUniforms matUni;
	GLint scaleUniform;
	GLint instancedUniform;
};

class ForwardShader{

private:
	static GLuint m_handle;
	ForwardShaderUniforms m_uniforms;
public:
	ForwardShader(){};
	bool CreateForwardShader();
	static GLuint GetHandle(){ return m_handle; }
	const ForwardShaderUniforms& GetUniforms()const { return m_uniforms; }
};