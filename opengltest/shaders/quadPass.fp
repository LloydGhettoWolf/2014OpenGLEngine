#version 330
uniform sampler2D ambient;
uniform sampler2D buffer;
uniform vec2 screenSize;
out vec4 color;

vec2 CalcCoord();

void main(){
  vec2 texCoords = CalcCoord();
  color = texture2D(ambient,texCoords);
  color += texture2D(buffer,texCoords);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}