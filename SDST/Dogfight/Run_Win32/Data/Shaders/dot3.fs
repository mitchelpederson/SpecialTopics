#version 420 core

#define MAX_LIGHTS 8
uniform vec3 EYE_POSITION;

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;


uniform vec3 LIGHT_POSITION[MAX_LIGHTS];
uniform vec3 LIGHT_DIRECTION[MAX_LIGHTS];
uniform vec4 LIGHT_COLOR[MAX_LIGHTS];
uniform float LIGHT_INTENSITY[MAX_LIGHTS];
uniform float LIGHT_ATTENUATION[MAX_LIGHTS];
uniform float LIGHT_IS_POINT[MAX_LIGHTS];
uniform float LIGHT_ANGLE[MAX_LIGHTS];

uniform float SPECULAR_POWER;
uniform float SPECULAR_AMOUNT;

layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec3 passWorldNormal;
in vec4 passColor;
in vec4 passWorldPos;


vec3 CalculateDot3Lighting( vec3 directionToLight, vec3 lightDirection, float lightDistance, float lightAttenuation, vec4 lightColor, float lightIntensity, float lightAngle, vec3 surfaceNormal, float isPoint, float cosPhi ) {

	vec3 actualLightDirection = mix( -lightDirection, directionToLight, isPoint);
	float cosHalfTheta = cos(radians(lightAngle * 0.5f));

	if (cosPhi < cosHalfTheta) {

		float dot3 = dot( actualLightDirection, surfaceNormal );
		vec3 lightContribution = max(dot3, 0.0f) * lightColor.rgb * lightIntensity * (1.f / (1.f + (lightAttenuation * lightDistance)));

		return lightContribution;
	}

	else {
		return vec3(0.f);
	}

}


vec3 CalculateSpecularLighting( vec3 lightPos, vec3 lightColor, float lightIntensity, float lightAttenuation, vec3 eyePos, vec4 worldPos ) {

	vec3 eyeDirection = normalize( EYE_POSITION - worldPos.xyz );
	vec3 lightDirection = normalize( lightPos - worldPos.xyz );
	float lightDistance = length(lightPos - worldPos.xyz);

	// Get the direction of the reflected light and how well it aligns 
	// with the direction of the camera
	vec3 reflectedLightDirection = reflect( -lightDirection, normalize(passWorldNormal) );
	float specularFactor = max( 0, dot( eyeDirection, reflectedLightDirection ) );

	// apply the specular factor to the specular model equation
	specularFactor = SPECULAR_AMOUNT * pow( specularFactor, SPECULAR_POWER );
	vec3 reflectedLight = lightColor.rgb * specularFactor * lightIntensity * (1.f / (1.f + lightAttenuation * lightDistance));
	return reflectedLight;
}


out vec4 outColor;

void main(void) {

	// Get the normal and the diffuse color
	vec3 surfaceNormal = normalize(passWorldNormal);
	vec4 surfaceColor = texture(gTexDiffuse, passUV) * passColor;


	// calculate ambient light
	vec3 surfaceLight = AMBIENT_COLOR.rgb * AMBIENT_INTENSITY;

	vec3 reflectedLight = vec3(0);

	for (int i = 0; i < MAX_LIGHTS; i++) {

		float lightDistance = length( LIGHT_POSITION[i] - passWorldPos.xyz );
		vec3 directionToLight = normalize( LIGHT_POSITION[i] - passWorldPos.xyz );

		float cosPhi = dot( normalize(passWorldPos.xyz - LIGHT_POSITION[i]), LIGHT_DIRECTION[i] );

		surfaceLight += CalculateDot3Lighting(directionToLight, LIGHT_DIRECTION[i], lightDistance, LIGHT_ATTENUATION[i], LIGHT_COLOR[i], LIGHT_INTENSITY[i], LIGHT_ANGLE[i], surfaceNormal, LIGHT_IS_POINT[i], cosPhi);
		reflectedLight += CalculateSpecularLighting( LIGHT_POSITION[i], LIGHT_COLOR[i].rgb, LIGHT_INTENSITY[i], LIGHT_ATTENUATION[i], EYE_POSITION, passWorldPos );
	}
	surfaceLight = clamp(surfaceLight, vec3(0), vec3(1));
	
	vec4 finalColor = vec4(surfaceLight, 1) * surfaceColor + vec4(reflectedLight, 0);

	//vec3 colorRgb = diffuse.rgb * passColor.rgb * AMBIENT_COLOR.rgb * AMBIENT_INTENSITY;
	outColor = finalColor;
}