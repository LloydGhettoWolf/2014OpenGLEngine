#version 400
uniform sampler2D tex;
uniform sampler2D bumpMap;
uniform sampler2D specMap;
uniform sampler2D alphaMap;
uniform sampler2D shadowTexture;

uniform bool useNormalMap;
uniform bool useAlphaMap;

uniform vec3 lightDir;
uniform vec3 lightCol;

uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;

uniform float materialShininess;

in vec3  normal;
in vec3  tangent;
in vec3  bitangent;
in vec3  eyeVec;
in vec2  UVs;
in vec4 shadowPosition;
out vec4 color;


void main(){

	float xOffset = 1.0/1024.0;
	float yOffset = 1.0/768.0;

	float shadowFactor = 0.0;

	for(float x = -1.5; x <= 1.5; x += 0.5){
		for(float y = - 1.5; y <= 1.5; y += 0.5){

			vec2 UVOffset = vec2(x*xOffset,y*yOffset);
			float depth = texture(shadowTexture,shadowPosition.xy + UVOffset).z;

			if(depth < shadowPosition.z - 0.001){
				shadowFactor  += 1.3;
			}
		}
	}

	if(shadowFactor > 0.0)
	  shadowFactor /= 81.0;
	
	mat3 TBNMatrix = mat3(normalize(tangent),normalize(bitangent),normalize(normal));

	vec3  norm;	
			  
	if(useNormalMap){
	  vec3 unpackedNormal = texture2D(bumpMap,UVs).xyz;
	  unpackedNormal = 2.0 * unpackedNormal - 1.0;
	  norm = normalize(TBNMatrix * unpackedNormal);
	}else{
	  norm = normalize(normal);
	}

    vec3  normalizedLight = normalize(-lightDir);
    vec3  halfVec		  = normalize((normalizedLight + normalize(eyeVec)));
    vec3  spec			  = pow(max(dot(halfVec,norm),0.0),materialShininess) * texture2D(specMap,UVs).xyz;
    vec3  diffCon	      = max(dot(norm,normalizedLight),0.0) * materialDiffuse;

	vec3 lighting =  lightCol * ( diffCon  + spec);
	
	vec3 newCol   = (((1.0 - shadowFactor) * (lighting)) + materialAmbient * 0.6) * texture2D(tex,UVs).xyz;
	

	if(useAlphaMap &&  texture2D(alphaMap,UVs).r < 0.2){
		discard;
	}

	color = vec4(newCol,1.0);
}