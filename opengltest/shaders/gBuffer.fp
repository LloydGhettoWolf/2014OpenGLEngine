#version 330
uniform vec3 materialDiffuse;
uniform vec3 materialAmbient;

in vec3 worldPos;
in vec3 normals;
in vec2 UVs;
layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 normalsOut;
layout(location = 2) out vec3 ambientOut;
layout(location = 3) out vec3 diffuseOut;

void main(){
  worldPosOut  = worldPos;
  normalsOut   = normals;
  ambientOut   = materialAmbient;
  diffuseOut   = materialDiffuse;
}