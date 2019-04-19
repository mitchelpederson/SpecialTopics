#version 420 core

#define MAX_LIGHTS 8

uniform mat4 MODEL;
uniform mat4 VIEW;

uniform vec3 EYE_POSITION;
uniform vec3 EYE_DIRECTION;

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

uniform vec3 LIGHT_POSITION[MAX_LIGHTS];
uniform vec3 LIGHT_DIRECTION[MAX_LIGHTS];
uniform vec4 LIGHT_COLOR[MAX_LIGHTS];
uniform float LIGHT_INTENSITY[MAX_LIGHTS];
uniform float LIGHT_ATTENUATION[MAX_LIGHTS];
uniform float LIGHT_IS_POINT[MAX_LIGHTS];
uniform float LIGHT_INNER_ANGLE[MAX_LIGHTS];
uniform float LIGHT_OUTER_ANGLE[MAX_LIGHTS];

uniform float LIGHT_IS_SHADOWCASTING[MAX_LIGHTS];
uniform mat4 SHADOW_VP[MAX_LIGHTS];
uniform mat4 SHADOW_INVERSE_VP[MAX_LIGHTS];

uniform float SPECULAR_POWER;
uniform float SPECULAR_AMOUNT;

uniform float MAX_FOG_DISTANCE;
uniform float FOG_FACTOR;
uniform vec4 FOG_COLOR;

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
layout(binding = 8) uniform sampler2DShadow gTexShadow;

in vec2 passUV;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;
in vec4 passColor;
in vec4 passWorldPos;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;


void main(void) {

	// Sample our textures
	vec4 texColor = texture(gTexDiffuse, passUV);
	texColor = texColor * passColor; // tint
	vec4 surfaceNormalSample = texture(gTexNormal, passUV);
	vec3 surfaceNormalTex = normalize( ( normalize(vec3(surfaceNormalSample.xyz)) * vec3(2.0, 2.0, 2.0)) - vec3(1.0, 1.0, 1.0) );

	// Get vertex TBN
	vec3 vertexWorldNormal = normalize( passNormal );
	vec3 vertexWorldTangent = normalize( passTangent );
	vec3 vertexWorldBitangent = normalize( passBitangent );
	mat3 tbn = mat3( vertexWorldTangent, vertexWorldBitangent, vertexWorldNormal );

	mat4 invModel = inverse( MODEL );
	// transform surface normal into worldspace
	vec3 surfaceNormal = (MODEL * vec4( normalize( vertexWorldNormal ), 0.0)).xyz ;

	vec3 eyeDirection =  EYE_POSITION - vec3(passWorldPos.xyz);
	eyeDirection = normalize( eyeDirection );

	vec3 ambientLight = vec3(AMBIENT_COLOR.xyz) * AMBIENT_INTENSITY;

	vec3 diffuseLight = vec3(texColor.xyz) * ambientLight;
	vec3 specularLight = vec3(0.0f);

	for (int i = 0; i < MAX_LIGHTS; i++) {

		float isLit = 1.0f;
		if (LIGHT_IS_SHADOWCASTING[i] > 0.1f) {
			vec4 clip = SHADOW_VP[i] * passWorldPos;
			vec3 ndc = clip.xyz / clip.w;
			ndc = (ndc + vec3(1.0f)) * 0.5f;

			float bias = 0.0005f * tan( acos( dot( surfaceNormal, LIGHT_DIRECTION[i] * -1.0f ) ) );
			ndc.z = ndc.z - bias;

			isLit = texture( gTexShadow, ndc );
		
		}

		// Light direction and distance
		vec3 directionToLight = LIGHT_POSITION[i] - vec3(passWorldPos.xyz);
		float lightDistance = length( directionToLight );
		directionToLight = normalize( directionToLight );

		//float cosAngleOfLight = dot( -directionToLight, normalize(LIGHT_DIRECTION[i]) );
		//float coneFalloff = smoothstep( cos(radians(LIGHT_OUTER_ANGLE[i] * 0.5)), cos(radians(LIGHT_INNER_ANGLE[i] * 0.5)), cosAngleOfLight );

		// Do diffuse
		float attenuation = 1.f / (1.f + (LIGHT_ATTENUATION[i] * lightDistance));
		vec3 actualLightDirection = mix( -normalize( LIGHT_DIRECTION[i] ), directionToLight, LIGHT_IS_POINT[i] );
		float dot3 = dot( actualLightDirection, surfaceNormal ) * LIGHT_INTENSITY[i] * attenuation;
		float diffuseDot3 = clamp(dot3, 0.0, 1.0);
		diffuseLight += vec3( LIGHT_COLOR[i].xyz * diffuseDot3 ) * isLit;

		// Do specular
		vec3 reflectedLightDirection = reflect( -actualLightDirection, surfaceNormal );
		float specularFactor = dot( reflectedLightDirection, eyeDirection );
		specularFactor = max(specularFactor, 0.0);
		specularFactor = SPECULAR_AMOUNT * pow(specularFactor, SPECULAR_POWER); 
		vec3 specularContribution = (attenuation * specularFactor * LIGHT_INTENSITY[i]) * LIGHT_COLOR[i].xyz;
		specularLight += specularContribution * isLit;
	}

	vec4 viewPosition = VIEW * passWorldPos;
	float fog = smoothstep( 0.0, MAX_FOG_DISTANCE, viewPosition.z);
	float fogFactor = FOG_FACTOR * fog;
	vec4 color = vec4(diffuseLight, 1) * texColor + vec4(specularLight, 0);

	fragColor = mix(color, FOG_COLOR, fogFactor);
	if ( dot( color.rgb, vec3( 0.2126, 0.7152, 0.0722)) > 1.0 ) { // normalization values taken from learnopengl.com/Advanced-Lighting/Bloom
		bloomColor = vec4((fragColor.rgb - vec3(1.0)), 1.0);
	} else {
		bloomColor = vec4(0.0,0.0,0.0,0.0);
	}


}