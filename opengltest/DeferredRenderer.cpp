#include "DeferredRenderer.h"
#include "CubeMap.h"
#include <gtc\matrix_transform.hpp>
#include <gtc\matrix_inverse.hpp>
#include <iostream>
using namespace std;

bool DeferredRenderer::Init(){

	if (!CreateGBuffer()){
		return false;
	}

	if (!m_deferredShader.CreateDeferredShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_sphereMesh, "sphere.obj", "meshes\\sphere\\")){
		cout << "couldn't load sphere mesh!" << endl;
		return false;
	}

	vec3 screenQuad[6];

	screenQuad[0] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[1] = vec3(-1.0f, 1.0f, 0.0f);
	screenQuad[2] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[3] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[4] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[5] = vec3(1.0f, -1.0f, 0.0f);

	glGenBuffers(1, &m_quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6, screenQuad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void DeferredRenderer::SetUniformsFirstTime(vec2& screenSize, mat3& normalMatrix, mat4& rotation){

	glUseProgram(m_deferredShader.GetGBufferHandle());
		mat4x4 translationMatrix = rotation;
		GBufferShaderUniforms gBufferUniforms = m_deferredShader.GetGBufferUniforms();
		glUniformMatrix3fv(gBufferUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
		glUniformMatrix4fv(gBufferUniforms.rotationMatrixUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);


	glUseProgram(m_deferredShader.GetLightPassHandle());
		glUniform2fv(m_deferredShader.GetLightPassUniforms().screenSizeUniform, 1, &screenSize[0]);
	glUseProgram(0);

	glUseProgram(m_deferredShader.GetQuadPassHandle());
		glUniform2fv(m_deferredShader.GetQuadPassUniforms().screenSizeUniform, 1, &screenSize[0]);
	glUseProgram(0);

}

void DeferredRenderer::BindFBO(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gBuffer.fboObject);
}

void DeferredRenderer::UnbindFBO(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredRenderer::PrepareGeometry(){
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(NUM_MRT, DrawBuffers);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

}

void DeferredRenderer::PresentToScreen(){


	//render to screen
	QuadPassShaderUniforms uniforms = m_deferredShader.GetQuadPassUniforms();

	glUseProgram(m_deferredShader.GetQuadPassHandle());
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(uniforms.bufferUniform, 0);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.finalTexture);

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(uniforms.ambTextureUniform, 1);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[2]);


		vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);

		glUniform2fv(uniforms.screenSizeUniform, 1, &screenSize[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);


}

void DeferredRenderer::RenderLights(mat4& viewProjection,PointLightData& lightData,vec3& camPos,int numLights){

	glDepthMask(GL_FALSE);
	mat4 worldMatrix, identity;
	MaterialUniforms matuni;

	vec3*		 lightPositions = lightData.position;
	vec3*		 lightColors    = lightData.color;
	Attenuation* attenuation    = lightData.attData;

	for (int light = 0; light < numLights; light++){
		glDrawBuffer(GL_NONE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		glUseProgram(m_deferredShader.GetNULLPassHandle());

			float scale = CalcSphereDistance(lightData, light);
			worldMatrix = viewProjection * glm::translate(identity, lightPositions[light]) * glm::scale(identity, vec3(scale, scale, scale));
			glUniformMatrix4fv(m_deferredShader.GetNULLWVPMatrix(), 1, false, &worldMatrix[0][0]);
			RenderStaticMeshComponent(m_sphereMesh.m_meshData[0]);

		glUseProgram(0);

		//lighting pass
		glDisable(GL_DEPTH_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);

		glUseProgram(m_deferredShader.GetLightPassHandle());
			LightPassShaderUniforms lightPassUniforms = m_deferredShader.GetLightPassUniforms();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[0]);
			glUniform1i(lightPassUniforms.posTextureUniform, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[1]);
			glUniform1i(lightPassUniforms.normTextureUniform, 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[2]);
			glUniform1i(lightPassUniforms.diffTextureUniform, 2);

			
			worldMatrix = translate(identity, lightPositions[light]) * glm::scale(identity, vec3(scale, scale, scale));
			glUniformMatrix4fv(lightPassUniforms.wvpMatrixUniform, 1, GL_FALSE, &(viewProjection * worldMatrix)[0][0]);

			glUniform3fv(lightPassUniforms.lightPosUniform, 1, &lightPositions[light][0]);
			glUniform3fv(lightPassUniforms.lightColUniform, 1, &lightColors[light][0]);
			glUniform3fv(lightPassUniforms.eyePosUniform, 1, &(camPos[0]));

			glUniform1f(lightPassUniforms.constAtt,  attenuation[light].constantAtt);
			glUniform1f(lightPassUniforms.linearAtt, attenuation[light].linearAtt);
			glUniform1f(lightPassUniforms.expAtt,    attenuation[light].expAtt);
			RenderStaticMeshComponent(m_sphereMesh.m_meshData[0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);


			glDisable(GL_BLEND);
			glCullFace(GL_BACK);
	}
	glDisable(GL_STENCIL_TEST);
}


bool DeferredRenderer::CreateGBuffer(){
	return CreateGBufferData(m_gBuffer);
}

float DeferredRenderer::CalcSphereDistance(const PointLightData& pLight, int index){
	float maxChan = std::max(std::max(pLight.color[index].r, pLight.color[index].g), pLight.color[index].b);

	float ret = (-pLight.attData[index].linearAtt + sqrtf(pLight.attData[index].linearAtt * pLight.attData[index].linearAtt
		- 4.0f *pLight.attData[index].expAtt * (pLight.attData[index].expAtt - 256.0f * maxChan))) / (2.0f*pLight.attData[index].expAtt);

	return ret;
}


