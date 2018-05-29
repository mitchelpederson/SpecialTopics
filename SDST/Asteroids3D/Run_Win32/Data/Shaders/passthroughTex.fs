#version 420 core

layout(binding = 0) uniform sampler2D gTexDiffuse;
uniform vec4 tint;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;

void main(void) {

	vec4 diffuse = texture(gTexDiffuse, passUV);

	outColor = diffuse * passColor;

}