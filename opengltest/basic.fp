#version 330
uniform sampler2D tex;
uniform sampler2D normalMap;
in vec2 uv;
in float dist;
out vec4 color;

void main(){
	color = texture2D(tex,uv);
}