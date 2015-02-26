//cookTorrance.fp
#version 400
uniform vec3 lightDir;
uniform vec3 lightCol;
uniform float roughness;
uniform float fresnel;
uniform float k;

in vec3 norm;
in vec3 eyeVec;

out vec4 color;

void main(){
  vec3 newNormal = normalize(norm);

  float NdotL = max(dot(newNormal,lightDir),0.0);

  float spec = 0.0;

  if(NdotL > 0.0){
    vec3 newEyeVec = normalize(eyeVec);

	vec3 halfVec = normalize(lightDir+newEyeVec);
	
	float NdotH =  max(dot(newNormal,halfVec),0.0);
	float NdotV =  max(dot(newNormal,newEyeVec),0.0);
	float VdotH =  max(dot(halfVec,newEyeVec),0.0);
	float LdotH =  max(dot(lightDir,halfVec),0.0);

	//geometric attenuation
	float g1 = (2.0 * NdotH * NdotV)/ VdotH;
	float g2 = (2.0 * NdotH * NdotL)/ VdotH;
	float geoAtt = min(1.0,min(g1,g2));

	//roughness
	float r1 = 1.0 / (3.14129 * roughness * roughness * pow(NdotH, 4.0));
	float r2 = (NdotH * NdotH - 1.0) / (roughness * roughness * NdotH * NdotH);
	float newRoughness = r1 * exp(r2);

	//fresnel
	float newFresnel = pow(1.0-VdotH,5.0);
	newFresnel *= (1.0 - fresnel);
	newFresnel += fresnel;

	spec = (newFresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14129);
  }

  vec3 finalVal = vec3(1.0,0.0,0.0) *  NdotL * (k+spec*(1.0-k));

  color = vec4(finalVal,1.0);

}