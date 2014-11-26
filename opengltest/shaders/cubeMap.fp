#version 330
uniform samplerCube sampler;
in vec3 outUV;

out vec4 color;

void main(){
  color = texture(sampler,outUV);
}