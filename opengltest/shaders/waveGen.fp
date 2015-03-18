//waveGen.fp - simply create an initial spectrum to feed into a ifft to create a height
//map later on

#version 400

uniform float amplitude;
uniform vec2  waveDir;
uniform float velocity;
uniform float t;

uniform vec2  texSize;

out vec3 color;


//random num generator for range -1.0 to 1.0 with a normal distribution
float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


//create a value depending on the position of the fragment from the center of the 
//render target

float PhillipsSpectrum(vec2 k){
  float kLen = length(k);
  float kSq = dot(k,k);
  float Amp = amplitude;
  float L = (velocity*velocity)/9.81;
  float dir = dot(normalize(waveDir),k);

  float w = L/1000.0;

  return  Amp  * exp(-1.0/(kSq * L * L))/ (kSq * kSq ) * (dir*dir) * exp(-kSq * w * w);
}

void main(){
	
	//get screenpos - center is 0.0 and ranges from -0.5 to 0.5 in both
	//directions
	vec2 screenPos = vec2(-texSize/2.0 + gl_FragCoord.xy) * (2.0f*3.141529/1500.0);
	
	//get random Guass number
	vec2 randomGuass = vec2(rand(screenPos),rand(screenPos.yx));

	//use phillips spectrum as a filter depending on position in freq domain
	float Phil = sqrt(PhillipsSpectrum(screenPos));
	float coeff = 1.0/sqrt(2.0);

	vec2 result = (coeff * randomGuass * Phil);

	//create dispersion relation, store it in z
	float dispersion = sqrt(9.81 * length(screenPos));

	color = vec3(result,dispersion);
}