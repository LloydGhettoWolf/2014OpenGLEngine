#version 330
uniform sampler2D texture;

in vec2 texCoord;
out vec4 color;

void main(){
  color = texture2D(texture,texCoord);
  
  if(color.r < 0.1 && color.g < 0.1 && color.b < 0.1)
        discard;
}