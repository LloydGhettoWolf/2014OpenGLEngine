//GbufferData
#include <iostream>
#include "GBufferData.h"
#include "DepthBuffer.h"
#include "Defines.h"

bool CreateGBufferData(GBufferData& data){
	glGenFramebuffers(1, &data.fboObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, data.fboObject);
	glViewport(0, 0, APP_WIDTH, APP_HEIGHT);
	
	glGenTextures(1, data.textures);

	for (int renderTarget = 0; renderTarget < 3; renderTarget++){
		glBindTexture(GL_TEXTURE_2D, data.textures[renderTarget]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, APP_WIDTH, APP_HEIGHT, 0, GL_RGB, GL_FLOAT, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderTarget, GL_TEXTURE_2D,
							data.textures[renderTarget], 0);
	}

	if (!CreateDepthTexture(data.depthTexture,false)){
		return false;
	}
	
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, DrawBuffers);
	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "problem creating g buffer!" << std::endl;
		return false;
	}
	
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	//glBindTexture()

	return true;
}