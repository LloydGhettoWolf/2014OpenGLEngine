#version 330

uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D diffuse;
uniform vec2      screenSize;
uniform vec3      lightColor;
uniform vec3      lightPos;
uniform vec3      eyePos;
out vec4 color;

vec2 CalcCoord();
vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diff);
vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 lightCol,float Attenuation);

void main(){
        vec2 texCoord = CalcCoord();
        vec3 worldPos = texture2D(position,texCoord).xyz;
        vec3 norm     = texture2D(normal,texCoord).xyz;
        vec3 diff     = texture2D(diffuse,texCoord).xyz;
        vec3 result   = CalcPointLight(worldPos,norm,diff);

        color = vec4(result,1.0);
		//color = vec4(worldPos,1.0);
}

vec2 CalcCoord(){
        return gl_FragCoord.xy/screenSize;
}

vec3 CalcPointLight(vec3 worldPos,vec3 normal,vec3 diffuse){
        vec3 lightDir   = lightPos-worldPos;
        float dist      = length(lightDir);
        vec3 lightNorm  = normalize(lightDir);
        float Attenuation = 1.0 +  0.05 *  dist * dist;
        Attenuation = 1.0/max(1.0, Attenuation);
    return ColorPoint(lightNorm,worldPos,normal,diffuse,Attenuation).xzy;
}

vec4 ColorPoint(vec3 lightDir,vec3 worldPos,vec3 normal,vec3 diffuse,float Attenuation){
        float diffFactor = Attenuation * max(dot(normal,lightDir),0.0);
        vec3 viewDir     = normalize(eyePos -worldPos);
        vec3 halfVec     = normalize((lightDir + viewDir) * 0.5);
        float spec       = Attenuation * pow(max(dot(normal,halfVec),0.0),64.0f);
        vec3 col = diffuse * lightColor * diffFactor;
        col += spec * vec3(1.0,1.0,1.0);
        col = clamp(col,0.0,1.0);
        return vec4(col,1.0);
}