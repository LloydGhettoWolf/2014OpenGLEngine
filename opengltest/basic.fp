#version 330
uniform sampler2D tex;
uniform sampler2D normalMap;
in vec3 norm;
in vec3 lightVec;
in vec3 eyeVec;
in float shiny;
out vec4 color;

void main(){
        float amb = 0.1;
        vec3 halfVec = normalize((lightVec+eyeVec));
        norm = normalize(norm);
        float spec   = pow(max(dot(halfVec,norm),0.0),shiny);
        float diff   = max(dot(norm,lightVec),0.0);
        vec3 teapotDiffCol = vec3(0.0,0.6,0.2);
        vec3 teapotSpecCol = vec3(0.2,0.7,0.2);
        color = vec4((diff + amb) * teapotDiffCol,1.0);
	color += vec4(clamp(spec * teapotSpecCol,0.0,1.0),1.0);
	color = clamp(color,0.0,1.0);
}