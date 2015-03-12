#version 400

const int NUM_WAVES = 15;

uniform vec2  directions[NUM_WAVES];
uniform float amplitude[NUM_WAVES];
uniform float freq[NUM_WAVES];
uniform float rho[NUM_WAVES];
//uniform float qVar[NUM_WAVES];
uniform float t;

uniform vec2  texSize;

out vec3 color;

void main(){
	
	vec3 sums;

	vec2 screenPos = vec2(gl_FragCoord.x,gl_FragCoord.y);

	for(int i = 0; i < NUM_WAVES; ++i){
		float dotvec = dot(directions[i], screenPos);
		float cosVal = cos(freq[i] * dotvec + rho[i] * t);

		float coeff = 0.2 * amplitude[i] * cosVal;

		sums.x += screenPos.x/texSize.x + coeff * directions[i].x;
		sums.y += amplitude[i] * sin(freq[i] * dotvec + rho[i] * t);
		sums.z += screenPos.y/texSize.y + coeff * directions[i].y;
	}

	sums /= 15.0;
	sums += 10.0;
	sums /= 20.0;

	color = vec3(sums);
}