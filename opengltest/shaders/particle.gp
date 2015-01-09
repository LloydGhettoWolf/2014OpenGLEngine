#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float type0[];
in vec3  position0[];
in vec3  velocity0[];
in float age0[];

out float type1;
out vec3  position1;
out vec3  velocity1;
out float age1;

uniform float     gDeltaTimeMillis;
uniform float     gTime;
uniform sampler1D gRandomTexture;
uniform float     gLauncherLifetime;
uniform float     gShellLifetime;
uniform float     gSecondaryShellLifetime;

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

vec3 GetRandomDir(float TexCoord){
  vec3 Dir = texture(gRandomTexture,texCoord).xyz ;
  Dir -= vec3(0.5,0.5,0.5);
  return Dir;
}

void main(){
}