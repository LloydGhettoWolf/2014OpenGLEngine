#version 400
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

  mat3 TBN = mat3(tangents,biTangents,normals);

  vec3 normalMapNormal =  texture2D(normalMap,UVs).xyz; 
  normalMapNormal = 2.0 * normalMapNormal - 1.0;
  normalize(TBN * normalMapNormal);

  normalsOut   = normalize((TBN * normalMapNormal + 1.0) * 0.5);
  ambientOut   = materialAmbient;
  diffuseOut   = texture(myTexture,UVs).xyz;
}