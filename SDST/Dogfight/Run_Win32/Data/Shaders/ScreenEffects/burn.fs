#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
uniform vec4 tint;
uniform float burnAmount;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;

void main(void) {

	vec4 texColor = texture(gTexDiffuse, passUV);

	outColor = vec4(0.5, 0.7, 1.0, burnAmount);

}