#version 330
uniform sampler2D ambient;
uniform vec2 screenSize;
out vec4 color;

vec2 CalcCoord();

void main(){
  vec2 texCoords = CalcCoord();
  color = texture2D(ambient,texCoords);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}