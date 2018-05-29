#pragma once
#include "Engine/Math/Vector2.hpp"

class Trajectory {
public:
	static Vector2	Evaluate( float gravity, Vector2 launchVelocity, float time );
	static Vector2	Evaluate( float gravity, float launchSpeed, float launchAngle, float time );
	static float	GetMinimumLaunchSpeed( float gravity, float distance );
	static bool		GetLaunchAngles(  float& out_minAngle
									, float& out_maxAngle
									, float gravity
									, float launchSpeed
									, float distance
									, float height = 0.0f );
	static float	GetMaxHeight( float gravity, float launchSpeed, float distance );
	static Vector2	GetLaunchVelocity( float gravity, float apexHeight, float distance, float height );
};