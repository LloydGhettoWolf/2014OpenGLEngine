//ParticleSystem.cpp
#include "ParticleSystem.h"
#include <string.h>

bool ParticleSystem::InitParticleSystem(const glm::vec3& pos){
	Particle particles[MAX_PARTICLES];

	memset(&particles[0], 0.0f, sizeof(particles));

	particles[0].type = PARTICLE_TYPE_LAUNCHER;
	particles[0].pos  = pos;
	particles[0].vel = glm::vec3(0.0f,0.0f,0.0f);
	particles[0].lifetimeMillisecs = 0.0f;

	glGenTransformFeedbacks(2, mTransformFeedback);
	glGenBuffers(2, mParticleBuffer);

	for (int buff = 0; buff < 2; buff++){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedback[buff]);
		glBindBuffer(GL_ARRAY_BUFFER,mParticleBuffer[buff]);
		glBufferData(GL_ARRAY_BUFFER,sizeof(particles),particles,GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,mParticleBuffer[buff]);
	}

	return true;
}

void ParticleSystem::Render(int deltaTime, const glm::mat4x4& mvp, const glm::vec3& camPos){
	mTime += deltaTime;
	UpdateParticles(deltaTime);
	RenderParticles(mvp,camPos);
	mCurrVB = mCurrTFB;
	mCurrTFB = (mCurrTFB + 1) & 0x1;
}

void ParticleSystem::UpdateParticles(int deltaTimeMillis){
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer[mCurrVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedback[mCurrTFB]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);

	glBeginTransformFeedback(GL_POINTS);

	if (mIsFirst){
		glDrawArrays(GL_POINTS, 0, 1);
		mIsFirst = false;
	}
	else{
		glDrawTransformFeedback(GL_POINTS, mTransformFeedback[mCurrVB]);
	}
}

void ParticleSystem::RenderParticles(const glm::mat4x4& mvp, const glm::vec3& camPos){

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedback[mCurrTFB]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);

	glDrawTransformFeedback(GL_POINTS, mTransformFeedback[mCurrTFB]);
	glDisableVertexAttribArray(0);
}