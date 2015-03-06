//GbufferData
#include <iostream>
#include "GBufferData.h"
#include "DepthBuffer.h"
#include "Defines.h"

GBufferData::~GBufferData(){
	glDeleteTextures(NUM_MRT, textures);
	glDeleteTextures(1, &depthTexture);
	glDeleteTextures(1, &finalTexture);
	glDeleteFramebuffers(1, &fboObject);
}

bool CreateGBufferData(GBufferData& data){
	GLenum format, type;

	glGenFramebuffers(1, &data.fboObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, data.fboObject);
	
	glGenTextures(NUM_MRT, data.textures);

	//glGetInternalformativ(GL_TEXTURE_2D, GL_RGB32F, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);

	for (int renderTarget = 0; renderTarget < NUM_MRT; renderTarget++){
		glBindTexture(GL_TEXTURE_2D, data.textures[renderTarget]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,APP_WIDTH, APP_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderTarget, GL_TEXTURE_2D,
							data.textures[renderTarget], 0);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	// depth
	glGenTextures(1, &data.depthTexture);
	glBindTexture(GL_TEXTURE_2D, data.depthTexture);
	glGetInternalformativ(GL_TEXTURE_2D, GL_DEPTH_STENCIL_NV, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_NV, APP_WIDTH, APP_HEIGHT, 0, GL_DEPTH_STENCIL, type,
					NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, data.depthTexture, 0);

	//experiment
	//glGetInternalformativ(GL_RENDERBUFFER, GL_DEPTH_STENCIL_NV, GL_RENDERBUFFER_INTERNAL_FORMAT, 1, (GLint*)&type);
	//glRenderbufferStorage(GL_RENDERBUFFER,type,APP_WIDTH,APP_HEIGHT);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL, GL_RENDERBUFFER, 0);


	// final
	glGenTextures(1, &data.finalTexture);
	glBindTexture(GL_TEXTURE_2D, data.finalTexture);
	glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, APP_WIDTH, APP_HEIGHT, 0, GL_RGBA, type, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, data.finalTexture, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "problem creating g buffer!" << std::endl;
		return false;
	}
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}