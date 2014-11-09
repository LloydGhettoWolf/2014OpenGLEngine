#version 330
uniform sampler2D tex;
uniform sampler2D normalMap;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;
in vec3  norm;
in vec3  lightVec;
in vec3  eyeVec;
in float shiny;
out vec4 color;

void main(){
        vec3 halfVec = normalize((lightVec+eyeVec));
	norm = normalize(norm);
        float spec   = pow(max(dot(halfVec,norm),0.0),shiny);
        float diff   = max(dot(norm,lightVec),0.0);
        vec3 ambContribution =  materialAmbient;
        vec3 diffContribution = (diff) * materialDiffuse;
	vec3 specContribution = spec * materialSpecular;
	color = vec4(ambContribution+specContribution+diffContribution,1.0);
	color = clamp(color,0.0,1.0);
}