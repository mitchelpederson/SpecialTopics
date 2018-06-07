#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
uniform vec4 tint;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;

void main(void) {

	vec4 texColor = texture(gTexDiffuse, passUV);
	texColor = texColor + vec4(0.05, 0.0, 0.05, 0.5);

	outColor = texColor;

}