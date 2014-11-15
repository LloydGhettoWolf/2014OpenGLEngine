#version 330
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;

in vec3 worldPos;
in vec3 normals;
in vec2 UVs;
layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 normalsOut;
layout(location = 2) out vec3 materialsOut;

void main(){
  worldPosOut  = clamp(worldPos,0.0,1.0);
  normalsOut   = clamp(normalize(normals),0.0,1.0);
  materialsOut = clamp(materialDiffuse+materialSpecular+materialAmbient,0.0,1.0);
}