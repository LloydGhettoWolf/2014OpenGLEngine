//waveUpdate.fp
#version 400

uniform float t;
uniform vec2  texSize;
uniform sampler2D spectrumTex;

layout(location = 1) out vec4 htOut;
layout(location = 2) out vec4 dXdTdYdTOut;

void main(){
	//sample the original frequency domain texture at two points - 
	//the corresponding texCoords and the inverse
	vec2 texCoord = gl_FragCoord.xy/texSize;
	vec2 otherTexCoord = vec2(1.0,1.0) - texCoord;


	vec3 h0_k   = texture(spectrumTex,texCoord).xyz;
	vec2 h0_mk  = texture(spectrumTex,otherTexCoord).xy;

	//get sin and cos values for current time t
	float sinV = sin(t);
	float cosV = cos(t);

	//get the real and imaginary parts into a vector
	vec2 ht;
	ht.x = (h0_k.x + h0_mk.x) * cosV - (h0_k.y + h0_mk.y) * sinV;
	ht.y = (h0_k.x - h0_mk.x) * sinV + (h0_k.y - h0_mk.y) * cosV;

	//get length from centre of freq domain
	//H(t) -> Dx(t), Dy(t)
	vec2 k = gl_FragCoord.xy - texSize * 0.5;
	float sqK = dot(k,k);

	float rsqr_k = 0.0;
	
	if(sqK > 1e-12){
	  rsqr_k = 1.0 /sqrt(sqK);
	}

	k *= rsqr_k;

	//find dX/dt and dY/dt
	vec2 dtX = vec2(ht.y * k.x, -ht.x * k.x);
	vec2 dtY = vec2(ht.y * k.y, -ht.x * k.y);

	//two render targets
	htOut       = vec4(ht,0.0,1.0);
	dXdTdYdTOut = vec4(dtX,dtY);
}