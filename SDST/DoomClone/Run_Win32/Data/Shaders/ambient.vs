#version 420 core

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
in vec3 NORMAL;

out vec2 passUV;
out vec3 passNormal;
out vec4 passColor;

void main (void) {

	vec4 localPosition = vec4(POSITION, 1);

	passUV = UV;
	passColor = COLOR;
	passNormal = NORMAL;


	gl_Position = PROJECTION * VIEW * MODEL * localPosition;

}