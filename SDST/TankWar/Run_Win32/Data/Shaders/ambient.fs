#version 420 core

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec3 passNormal;
in vec4 passColor;


out vec4 outColor;

void main(void) {

	vec4 diffuse = texture(gTexDiffuse, passUV);
	vec3 colorRgb = diffuse.rgb * passColor.rgb * AMBIENT_COLOR.rgb * AMBIENT_INTENSITY;

	outColor = vec4(colorRgb, diffuse.a);
}