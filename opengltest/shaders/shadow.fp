//shadow.fp
#version 400
uniform sampler2D shadowTexture;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;
uniform float shininess;

in vec3  norm;
in vec3  eyeVec;
in vec3  lightVec;
in vec4  shadowCoords;

out vec4 color;

void main(){
	vec3 normal = normalize(norm);
        
    vec3  normalizedLight = normalize(lightVec);
    vec3  halfVec = normalize((normalizedLight+normalize(eyeVec)));
    
    float spec   = pow(max(dot(halfVec,norm),0.0),shininess);
    float diff   = max(dot(normal,normalizedLight),0.0);
    vec3  diffContribution = diff * materialDiffuse;
    vec3  specContribution = spec * materialSpecular;
	
	vec3 perspDivideShadow = shadowCoords.xyz/shadowCoords.w;

	float depth = texture(shadowTexture,perspDivideShadow.xy).z;

	float shadowCoeff = 1.0;
	float offset = 0.0;

	if(depth < perspDivideShadow.z-0.005 && shadowCoords.w > 0.0){
	   offset += 0.3;
	}
	
	shadowCoeff -= offset;

	vec3 lighting = shadowCoeff * (diffContribution + specContribution) + materialAmbient;
	
	color = vec4(lighting,1.0);
}