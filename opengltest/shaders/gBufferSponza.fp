uniform sampler2D specularMap;
uniform sampler2D alphaMap;

uniform bool useNormalMap;
uniform bool useAlphaMap;

in vec3 worldPos;
in vec3 normals;
in vec3 tangents;
in vec3 biTangents;
in vec2 UVs;

layout(location = 0) out vec4 worldPosOut;
layout(location = 1) out vec4 normalsOut;
layout(location = 2) out vec4 ambientOut;
layout(location = 3) out vec4 diffuseOut;

void main(){
	
  if(useAlphaMap &&  texture2D(alphaMap,UVs).r < 0.2){
		discard;
  }

  worldPosOut.xyz  = worldPos;

  if(useNormalMap){
	  mat3 TBN = mat3(tangents,biTangents,normals);

	  vec3 normalMapNormal =  texture2D(normalMap,UVs).xyz; 
	  normalMapNormal = 2.0 * normalMapNormal - 1.0;
	  normalsOut.xyz   = (normalize(TBN * normalMapNormal) + 1.0) * 0.5;
  }else{
	  normalsOut.xyz = normals;
  }

  diffuseOut.xyz   = texture(myTexture,UVs).xyz;
  ambientOut.xyz   = materialAmbient.xyz * diffuseOut.xyz;
  diffuseOut.w     = texture(specularMap,UVs).x;
}