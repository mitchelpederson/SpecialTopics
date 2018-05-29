#include "Engine/Math/Trajectory.hpp"
#include "Engine/Math/MathUtils.hpp"



Vector2 Trajectory::Evaluate(  float gravity, Vector2 launchVelocity, float time ) {
	float launchAngle = launchVelocity.GetOrientationDegrees();
	float launchSpeed = launchVelocity.GetLength();
	return Trajectory::Evaluate(gravity, launchSpeed, launchAngle, time);
}


Vector2 Trajectory::Evaluate( float gravity, float launchSpeed, float launchAngle, float time ) {
	Vector2 position;
	position.x = launchSpeed * CosDegrees(launchAngle) * time;
	position.y = (-0.5f * gravity * time * time) + (launchSpeed * SinDegrees(launchAngle) * time); 
	return position;
}


float Trajectory::GetMinimumLaunchSpeed( float gravity, float distance ) {
	Vector2 speedComponents;
	float angle = 45.f;
	float angleCos = CosDegrees(angle);
	float minT = 0.f;
	float maxT = 0.f;

	Quadratic(minT, maxT, -0.5f * gravity, (distance / angleCos) * SinDegrees(angle), 0.f);
	float speed = distance / (angleCos * maxT);
	return speed;
} 


bool Trajectory::GetLaunchAngles( float& out_minAngle , float& out_maxAngle , float gravity , float launchSpeed , float distance , float height /* = 0.0f */ ) {
	float min = 0.f;
	float max = 0.f;

	// Equation we are going to solve is:
	// 0 = (-gx^2)/(2v^2)tan^2(angle) + x*tan(angle) - (-gx^2)/(2v^2) - y
	// we will use the quadratic formula to solve for tan(angle) and then take the arctangent

	// this is the a part of the quadratic formula
	float a = (-gravity * distance * distance) / (2.f * launchSpeed * launchSpeed);
	float b = distance;
	float c = a - height;

	if (Quadratic(min, max, a, b, c)) {
		out_minAngle = min;
		out_maxAngle = max;
		return true;
	} else {
		return false;
	}
}


float Trajectory::GetMaxHeight( float gravity, float launchSpeed, float distance ) {
	// First find the angle of max height by finding solving for y' in terms of theta
	// y' = (-g*x^2/v^2)*tan(theta)*sec^2(theta) + x*sec^2(theta)
	// reduces to
	// y' = sec^2(theta) * (-(g*x^2/v^2)*tan(theta) + x)
	// set y' to zero to find the angle at which y is a max, sec^2 cancels out
	// 0 = (-g*x^2/v^2)*tan(theta) + x
	// arctan(v^2/g*x) = theta

	float theta = Atan2Degrees(launchSpeed * launchSpeed, gravity * distance);


	// Now use y = (-0.5 * g * t^2) + (v * sin(theta) * t)
	// substitute t = x / (v * cos(theta))
	// y = ( -0.5f * g * (v*cos(theta))^2 )  +  ( v * sin(theta) * (x / v * cos(theta)) )
	float cosTheta = CosDegrees(theta);
	float sinTheta = SinDegrees(theta);
	float gravityTerm = -0.5f * gravity * (distance / (launchSpeed * cosTheta)) * (distance / (launchSpeed * cosTheta));
	float otherTerm = distance * (sinTheta / cosTheta);

	return gravityTerm + otherTerm;


}