
#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 7) uniform sampler2D gTexDepth;
uniform vec4 tint;
uniform float burnAmount;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;

void main(void) {

	vec4 texColor = texture(gTexDiffuse, passUV);
	vec4 depth = texture(gTexDepth, passUV);

	outColor = depth;

}