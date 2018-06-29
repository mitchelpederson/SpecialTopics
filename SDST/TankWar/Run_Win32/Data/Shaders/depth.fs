#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
uniform vec4 tint;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;

void main(void) {

	float depth = (2.0 * gl_FragCoord.z) - 1.0;
	depth = 2.0 * 0.1 / (1000.1 - depth * (999.9));
	outColor = vec4(depth, depth, depth, 1);

}