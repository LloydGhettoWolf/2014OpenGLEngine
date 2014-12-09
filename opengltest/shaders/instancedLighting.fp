#version 330
const int    numLights = 1;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;
uniform float shininess;
uniform vec3 lightColors[numLights];

in vec3  norm;
in vec3  eyeVec;
in vec3  lightVector[numLights];
out vec4 color;

void main(){
        vec3 halfVec;
	vec3 normal = normalize(norm);
        vec3 spec = vec3(0.0,0.0,0.0);
        vec3 diff = vec3(0.0,0.0,0.0);
        for(int light = 0; light < numLights; light++){
            vec3 normalizedLight = normalize(lightVector[light]);
            halfVec = normalize((normalizedLight+eyeVec)* 0.5);
            float dist  = length(lightVector[light]);
            float atten = (1.0 / (0.1 +  0.01 * dist + (0.0001 * pow(dist,2.0))));
            spec   +=  atten * pow(max(dot(halfVec,normal),0.0),shininess) ;
            diff   +=  atten * max(dot(normal,normalizedLight),0.0) * lightColors[light];
        }
        diff = clamp(diff,0.0,1.0);
        spec = clamp(spec,0.0,1.0);
        vec3 diffContribution = diff * materialDiffuse;
	vec3 specContribution = spec * materialSpecular;
	vec3 lighting = (diffContribution+specContribution)+materialAmbient;
	color = vec4(lighting,1.0);
}