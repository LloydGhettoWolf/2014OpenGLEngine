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

	float xOffset = 1.0/1024.0;
	float yOffset = 1.0/768.0;

	float shadowFactor = 0.0;

	for(float x = -1.5; x <= 1.5; x += 0.5){
		for(float y = - 1.5; y <= 1.5; y += 0.5){

			vec2 UVOffset = vec2(x*xOffset,y*yOffset);
			float depth = texture(shadowTexture,perspDivideShadow.xy + UVOffset).z;

			if(depth < perspDivideShadow.z - 0.005){
				shadowFactor  += 0.3;
			}
		}
	}

	shadowFactor /= 81.0;

	vec3 lighting = (1.0 - shadowFactor)  * (diffContribution + specContribution) + materialAmbient;
	
	color = vec4(lighting,1.0);
}