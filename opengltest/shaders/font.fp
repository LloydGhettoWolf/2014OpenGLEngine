#version 330

uniform sampler2D fontTexture;
in vec2 outUV;

// Ouput data
out vec4 color;

void main(){
	vec4 testColor = texture2D( fontTexture, outUV );
	
	//if(testColor.rgb == vec3(1.0,0.0,0.0)) discard;
	
	color = testColor;
}