//comparison.fp
#version 400
uniform vec3  materialDiffuse;
uniform vec3  materialSpecular;
uniform vec3  materialAmbient;
uniform float shininess;
uniform float reflectFactor;
uniform float solidMix;
uniform vec3  lightDir;
uniform samplerCube cubeMap;

in vec3  norm;
in vec3  eyeVec;
in vec3  reflectDir;
in vec3  refractDir;
out vec4 color;

void main(){
	vec3 normal = normalize(norm);
   
    vec3 halfVec     = normalize(lightDir + normalize(eyeVec));
  
	vec3 spec   = pow(max(dot(halfVec,normal),0.0),shininess)* vec3(1.0f,1.0f,1.0f);
    vec3 diff   = max(dot(normal,lightDir),0.0) * vec3(1.0f,1.0f,1.0f);
    
    diff = clamp(diff * materialDiffuse,0.0,1.0);
    spec = clamp(spec * materialSpecular,0.0,1.0);
   
	vec3 lighting = diff + spec + materialAmbient;
	vec3 reflectColor = texture(cubeMap,normalize(reflectDir)).xyz;
	vec3 refractColor = texture(cubeMap,normalize(refractDir)).xyz;

	vec3 mixColor = mix(refractColor,reflectColor,reflectFactor);
	mixColor      = mix(lighting,mixColor,solidMix);

	color = vec4(mixColor,1.0);
}