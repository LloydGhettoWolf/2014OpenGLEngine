#version 400
uniform sampler2D buff;
uniform sampler2D diff;
uniform vec2 screenSize;
out vec4 color;

vec2 CalcCoord();

void main(){
  vec2 texCoords = CalcCoord();
  color = texture2D(buff,texCoords);
  color += 0.1 * texture2D(diff,texCoords);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}