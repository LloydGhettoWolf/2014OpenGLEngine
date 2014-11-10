#version 330
uniform sampler2D tex;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;
uniform float shininess;
in vec3  norm;
in vec3  lightVec;
in vec3  eyeVec;
in vec2  UVs;
out vec4 color;

void main(){
        vec3 halfVec = normalize((lightVec+eyeVec));
	vec3 normal = normalize(norm);
        float spec   = pow(max(dot(halfVec,norm),0.0),shininess);
        float diff   = max(dot(normal,lightVec),0.0);
        vec3 diffContribution = diff * materialDiffuse;
	vec3 specContribution = spec * materialSpecular;
	vec3 lighting = diffContribution+specContribution+materialAmbient;
	vec3 newCol   = lighting * texture2D(tex,UVs).xyz;
	color = vec4(newCol,1.0);
}