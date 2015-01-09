//GbufferData
#include <iostream>
#include "GBufferData.h"
#include "DepthBuffer.h"
#include "Defines.h"

bool CreateGBufferData(GBufferData& data){
	glGenFramebuffers(1, &data.fboObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, data.fboObject);
	glViewport(0, 0, APP_WIDTH, APP_HEIGHT);
	
	glGenTextures(NUM_MRT, data.textures);

	for (int renderTarget = 0; renderTarget < NUM_MRT; renderTarget++){
		glBindTexture(GL_TEXTURE_2D, data.textures[renderTarget]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, APP_WIDTH, APP_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderTarget, GL_TEXTURE_2D,
							data.textures[renderTarget], 0);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	if (!CreateDepthTexture(data.depthTexture,false,true)){
		return false;
	}

	// final
	glGenTextures(1,&data.finalTexture);
	glBindTexture(GL_TEXTURE_2D, data.finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, APP_WIDTH, APP_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, data.finalTexture, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(NUM_MRT, DrawBuffers);
	
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "problem creating g buffer!" << std::endl;
		return false;
	}
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}