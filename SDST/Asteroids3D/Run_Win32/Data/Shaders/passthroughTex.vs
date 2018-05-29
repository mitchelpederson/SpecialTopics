#version 420 core

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;

out vec2 passUV;
out vec4 passColor;

void main (void) {

	vec4 localPosition = vec4(POSITION, 1);

	passUV = UV;
	passColor = COLOR;

	gl_Position = PROJECTION * VIEW * MODEL * localPosition;

}