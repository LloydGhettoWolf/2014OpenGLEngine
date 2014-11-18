#version 330
const int numLights = 3;
uniform sampler2D tex;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform vec3 materialAmbient;
uniform vec3 lightColors[numLights];
uniform float shininess;
in vec3  norm;
in vec3  lightVec[numLights];
in vec3  eyeVec;
in vec2  UVs;
out vec4 color;

void main(){
	vec3 normal = normalize(norm);
        vec3 diffContribution = vec3(0.0,0.0,0.0);
        vec3 specContribution = vec3(0.0,0.0,0.0);
        for(int light = 0; light < numLights; light++){
            vec3 normalizedLight = normalize(lightVec[light]);
            vec3 halfVec = normalize((normalizedLight+eyeVec));
            float dist  = length(lightVec[light]);
            float atten = (1.0 / (0.1 +  0.01 * dist + (0.0001 * pow(dist,2.0))));
            float spec   = atten * pow(max(dot(halfVec,norm),0.0),shininess);
            float diff   = atten * max(dot(normal,normalizedLight),0.0);
            diffContribution += diff * materialDiffuse * lightColors[light];
    	    specContribution += spec * materialSpecular * lightColors[light];
	}
	vec3 lighting = diffContribution+specContribution+materialAmbient;
	//vec3 newCol   = lighting * texture2D(tex,UVs).xyz;
	color = vec4(lighting,1.0);
}