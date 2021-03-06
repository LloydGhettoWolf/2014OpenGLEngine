//ParticleSystem.h
#pragma once
#include <glm.hpp>
#include <GL\glew.h>
#include "PointSprite.h"
#include "ParticleShader.h"
#include "PointSpriteShader.h"

const int MAX_PARTICLES = 5000;

enum PARTICLE_TYPE {PARTICLE_TYPE_LAUNCHER};

class ParticleSystem{

	public:
		ParticleSystem():mCurrVB(0),mCurrTFB(0){};
		~ParticleSystem(){};
		bool InitParticleSystem(const glm::vec3& pos);
		void Render(int deltaTime, const glm::mat4x4& mvp, const glm::vec3& camPos);
		void UpdateParticles(int deltaTime);
	private:
		void RenderParticles(const glm::mat4x4& mvp, const glm::vec3& camPos);

		bool mIsFirst;
		unsigned int mCurrVB;
		unsigned int mCurrTFB;
		GLuint mParticleBuffer[2];
		GLuint mTransformFeedback[2];
		GLuint mTexture;
		int mTime;

		PointSprite sprite;

		PointSpriteShader mSpriteShader;
		ParticleShader    mParticleShader;
};

struct Particle{
	PARTICLE_TYPE type;
	glm::vec3 pos;
	glm::vec3 vel;
	float lifetimeMillisecs;
};