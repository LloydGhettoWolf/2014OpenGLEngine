#version 400

uniform sampler2D waveTexture;

in vec2 uvs;
in vec3 normals;
out vec4 color;


void main(){
	
	float diffVal = dot(normalize(normals),normalize(vec3(1.0,1.0,0.0)));

	vec3 rgb = texture(waveTexture,uvs).xyz;
	rgb *= 20.0;
	rgb -= 10.0;

	color = vec4(diffVal * vec3(0.0,0.9,0.9),1.0);

}