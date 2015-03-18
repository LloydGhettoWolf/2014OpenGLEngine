#version 400

//waveGen.fp - simply create an initial spectrum to feed into a ifft to create a height
//map later on

uniform float amplitude;
uniform vec2  waveDir;
uniform float velocity;
//uniform float rho[NUM_WAVES];
//uniform float qVar[NUM_WAVES];
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
  float kSq = kLen * kLen;
  float Amp = amplitude;
  float L = (velocity*velocity)/9.81;
  float dir = dot(normalize(waveDir),normalize(k));

  return Amp * (dir*dir) * exp(-1.0/(kSq * L * L))/ (kSq * kSq) ;
}

void main(){
	
	vec3 sums;

	//get screenpos - center is 0.0 and ranges from -0.5 to 0.5 in both
	//directions
	vec2 screenPos = vec2(gl_FragCoord.x,gl_FragCoord.y)/texSize - vec2(0.5,0.5);
	
	float period = (2.0 * 3.141519) / texSize.x;

	//get random Guass number
	vec2 randomGuass = period * vec2(rand(screenPos),rand(screenPos.yx));

	//use phillips spectrum as a filter depending on position in freq domain
	float Phil = sqrt(PhillipsSpectrum(screenPos));
	float coeff = 1.0/sqrt(2.0);

	vec2 result = (coeff * randomGuass * Phil);

	color = vec3(result,0.0);
}