#version 400
uniform sampler2D tex;
uniform vec3 lightDir;
uniform vec3 lightCol;

uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;

uniform float materialShininess;

in vec3  norm;
in vec3  eyeVec;
in vec2  UVs;
out vec4 color;


void main(){
	vec3 normal			 = normalize(norm);
    vec3 normalizedLight = normalize(-lightDir);
    vec3 halfVec		 = normalize((normalizedLight+normalize(eyeVec)));
    float spec			 = pow(max(dot(halfVec,normal),0.0),materialShininess);
    float diff			 = max(dot(normal,normalizedLight),0.0);

	vec3 lighting = lightCol *( diff * materialDiffuse  + spec * materialSpecular + materialAmbient);

	vec3 newCol   = lighting * texture2D(tex,UVs).xyz;

	color = vec4(newCol,1.0);
}