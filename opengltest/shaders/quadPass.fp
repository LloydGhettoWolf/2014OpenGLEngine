#version 400
uniform sampler2D buff;
uniform sampler2D amb;
uniform vec2 screenSize;
out vec4 color;

vec2 CalcCoord();

void main(){
  vec2 texCoords = CalcCoord();
  vec4 ambient = texture2D(amb,texCoords) * 0.5;
  color = texture2D(buff,texCoords) + ambient;
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}