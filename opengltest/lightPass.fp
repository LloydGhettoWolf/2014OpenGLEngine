#version 330

uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D diffuse;
uniform vec2      screenSize;
uniform vec3      lightColor;
uniform vec3      lightPos;

out vec4 color;

vec2 CalcCoord();
vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diff);
vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 lightCol);

void main(){
        vec2 texCoord = CalcCoord();
        vec3 worldPos = texture2D(position,texCoord).xyz;
        vec3 norm     = texture2D(normal,texCoord).xyz;
        vec3 diff     = texture2D(diffuse,texCoord).xyz;
        vec3 result   = CalcPointLight(worldPos,norm,diff);

        color = vec4(result,1.0);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}

vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diffuse){
        vec3 lightDir   = worldPos - lightPos;
        float dist      = length(lightDir);
        vec3 lightNorm  = normalize(lightDir);
        float Attenuation = 1.0 +  0.001 *  dist * dist;
        Attenuation = max(1.0, Attenuation);
    return ColorPoint(lightNorm,worldPos,normal,diffuse).xzy/ Attenuation;
}

vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 diffuse){
        float diffFactor =max(dot(normal,-lightDir),0.0);
        vec3 col = diffuse * lightColor * diffFactor;
        col = clamp(col,0.0,1.0);
        return vec4(col,1.0);
}