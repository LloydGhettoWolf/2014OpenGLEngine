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
	norm = normalize(norm);
        float spec   = pow(max(dot(halfVec,norm),0.0),shininess);
        float diff   = max(dot(norm,lightVec),0.0);
        vec3 ambContribution =  materialAmbient;
        vec3 diffContribution = (diff) * materialDiffuse;
	vec3 specContribution = spec   * materialSpecular;
	vec4 lighting = vec4(ambContribution+specContribution+diffContribution,1.0);
	color = lighting * texture2D(tex,UVs);
	//color = vec4(1.0,1.0,1.0,1.0);
}