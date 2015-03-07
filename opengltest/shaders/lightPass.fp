#version 400

uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D diffuse;
uniform vec2      screenSize;
uniform vec3      lightColor;
uniform vec3      lightPos;
uniform vec3      eyePos;
out vec4          color;

vec2 CalcCoord();
vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diff,float spec);
vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 lightCol,float Attenuation,float spec);

void main(){
        vec2 texCoord = CalcCoord();
        vec3 worldPos = texture(position,texCoord).xyz;
        vec3 norm     = normalize(texture2D(normal,texCoord).xyz * 2.0 - 1.0);
        vec4 diff     = texture(diffuse,texCoord);
        vec3 result   = CalcPointLight(worldPos,norm,diff.xyz,diff.w);
        color =       vec4(result,1.0);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}

vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diffuse,float spec){
        vec3 lightDir   = lightPos-worldPos;
        float dist      = length(lightDir);
        vec3 lightNorm  = normalize(lightDir);
        float Attenuation = 1.0 +  0.2 + 0.02 *  dist * dist;
        Attenuation = 1.0/max(1.0, Attenuation);
    return ColorPoint(lightNorm,worldPos,normal,diffuse,Attenuation,spec).xzy;
}

vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 diffuse,float Attenuation,float spec){
        float diffFactor = max(dot(normal,lightDir),0.0);
        vec3 viewDir     = normalize(eyePos - worldPos);
        vec3 halfVec     = normalize((lightDir + viewDir) * 0.5);
        float specFactor = spec * pow(max(dot(normal,halfVec),0.0),256.0f);
        vec3 col = lightColor * ((diffuse * diffFactor) + specFactor);
        col = clamp(Attenuation * col,0.0,1.0);
        return vec4(col,1.0);
}