#version 400
uniform sampler2D buff;
uniform vec2 screenSize;
out vec4 color;

vec2 CalcCoord();

void main(){
  vec2 texCoords = CalcCoord();
  color = texture2D(buff,texCoords);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}