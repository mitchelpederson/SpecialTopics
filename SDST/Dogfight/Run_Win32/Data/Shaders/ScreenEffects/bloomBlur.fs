// Gaussian blur for bloom, based on a tutorial at https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5

#version 420 core

layout(binding = 3) uniform sampler2D gTexBloom;

in vec2 passUV;
in vec4 passColor;

uniform vec3 blurDirection;
uniform vec3 SCREEN_DIMENSIONS;

out vec4 outColor;

void main(void) {

	vec4 sum = vec4(0.0);
	float blurAmount = 1.0 / max( mix(0.0, SCREEN_DIMENSIONS.x, blurDirection.x), mix( 0.0, SCREEN_DIMENSIONS.y, blurDirection.y ) );
	vec2 direction = blurDirection.xy;

	// Perform gaussian blur. Weights calculated with sigma = 1.9, kernel size = 13 from http://dev.theomader.com/gaussian-kernel-calculator/
	sum += texture( gTexBloom, vec2( passUV.x - (6.0 * blurAmount * direction.x), passUV.y - ( 6.0 * blurAmount * direction.y) ) ) * 0.004659;
	sum += texture( gTexBloom, vec2( passUV.x - (5.0 * blurAmount * direction.x), passUV.y - ( 5.0 * blurAmount * direction.y) ) ) * 0.01424;
	sum += texture( gTexBloom, vec2( passUV.x - (4.0 * blurAmount * direction.x), passUV.y - ( 4.0 * blurAmount * direction.y) ) ) * 0.035521;
	sum += texture( gTexBloom, vec2( passUV.x - (3.0 * blurAmount * direction.x), passUV.y - ( 3.0 * blurAmount * direction.y) ) ) * 0.072314;
	sum += texture( gTexBloom, vec2( passUV.x - (2.0 * blurAmount * direction.x), passUV.y - ( 2.0 * blurAmount * direction.y) ) ) * 0.120151;
	sum += texture( gTexBloom, vec2( passUV.x - (1.0 * blurAmount * direction.x), passUV.y - ( 1.0 * blurAmount * direction.y) ) ) * 0.162939;

	sum += texture( gTexBloom, vec2( passUV.x, passUV.y ) ) * 0.180353;

	sum += texture( gTexBloom, vec2( passUV.x + (6.0 * blurAmount * direction.x), passUV.y + ( 6.0 * blurAmount * direction.y) ) ) * 0.004659;
	sum += texture( gTexBloom, vec2( passUV.x + (5.0 * blurAmount * direction.x), passUV.y + ( 5.0 * blurAmount * direction.y) ) ) * 0.01424;
	sum += texture( gTexBloom, vec2( passUV.x + (4.0 * blurAmount * direction.x), passUV.y + ( 4.0 * blurAmount * direction.y) ) ) * 0.035521;
	sum += texture( gTexBloom, vec2( passUV.x + (3.0 * blurAmount * direction.x), passUV.y + ( 3.0 * blurAmount * direction.y) ) ) * 0.072314;
	sum += texture( gTexBloom, vec2( passUV.x + (2.0 * blurAmount * direction.x), passUV.y + ( 2.0 * blurAmount * direction.y) ) ) * 0.120151;
	sum += texture( gTexBloom, vec2( passUV.x + (1.0 * blurAmount * direction.x), passUV.y + ( 1.0 * blurAmount * direction.y) ) ) * 0.162939;

	outColor = sum;

	//outcolor = vec4(1.0, 0.0, 1.0, 1.0);

}