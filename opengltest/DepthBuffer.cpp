//DepthBuffer.cpp
#include "DepthBuffer.h"
#include <iostream>
#include "Defines.h"

bool CreateDepthTexture(GLuint& depthFBO, bool bindBuffer, bool stencil){

	GLuint depthTexture;

	if (bindBuffer){
		glGenFramebuffers(1, &depthFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthFBO);
	}

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	if (stencil){
		GLenum type;
		glGetInternalformativ(GL_TEXTURE_2D, GL_DEPTH32F_STENCIL8, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, APP_WIDTH, APP_HEIGHT, 0, GL_DEPTH_STENCIL,
			type, 0);
	}
	else{
		GLenum type;
		glGetInternalformativ(GL_TEXTURE_2D, GL_DEPTH, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH, APP_WIDTH, APP_HEIGHT, 0, GL_DEPTH,type, 0);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	if (stencil){
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	}
	else{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH, GL_TEXTURE_2D, depthTexture, 0);
	}

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "problem creating depth buffer!" << std::endl;
		return false;
	}
	
	if (bindBuffer){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}else{
		depthFBO = depthTexture;
	}

	return true;
}
