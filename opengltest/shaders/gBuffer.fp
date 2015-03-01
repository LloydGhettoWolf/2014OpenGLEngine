#version 330
uniform vec3 materialDiffuse;
uniform vec3 materialAmbient;
uniform sampler2D myTexture;
uniform sampler2D normalMap;

in vec3 worldPos;
in vec3 normals;
in vec3 tangents;
in vec3 biTangents;
in vec2 UVs;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 normalsOut;
layout(location = 2) out vec3 ambientOut;
layout(location = 3) out vec3 diffuseOut;

void main(){
  worldPosOut  = worldPos;

  mat3 TBN = mat3(tangents[0],biTangents[0],normals[0],
				  tangents[1],biTangents[1],normals[1],
				  tangents[2],biTangents[2],normals[2]);

  vec3 normalMapNormal = texture(normalMap,UVs).xyz;

  normalsOut   = normalize(TBN * normalMapNormal);
  ambientOut   = materialAmbient;
  diffuseOut   = texture(myTexture,UVs).xyz;
}