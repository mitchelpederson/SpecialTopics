#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
uniform vec4 tint;

in vec2 passUV;
in vec4 passColor;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 bloomColor;

void main(void) {

	vec4 diffuse = texture(gTexDiffuse, passUV);

	outColor = diffuse * passColor;
	bloomColor = vec4(diffuse * passColor * vec4(2.0)) - vec4(1, 1, 1, 0);

}