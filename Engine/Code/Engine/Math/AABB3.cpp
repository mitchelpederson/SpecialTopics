#include "Engine/Math/AABB3.hpp"

AABB3::AABB3() 
	: mins(Vector3(1.f, 1.f, 1.f))
	, maxs(Vector3(-1.f, -1.f, -1.f)) 
{
}


AABB3::AABB3( const Vector3& mins, const Vector3& maxs )
	: mins(mins)
	, maxs(maxs)
{
}


Vector3 AABB3::GetCenter() const {
	return Vector3( (mins.x + maxs.x) * 0.5f, (mins.y + maxs.y) * 0.5f, (mins.z + maxs.z) * 0.5f );
}


Vector3 AABB3::GetDimensions() const {
	return maxs - mins;
}


bool AABB3::Contains( const Vector3& point ) const {
	if (   mins.x <= point.x && mins.y <= point.y && mins.z <= point.z
		&& maxs.x >= point.x && maxs.y >= point.y && maxs.z >= point.z) {
		return true;
	}
	return false;
}

std::vector<RaycastHit3> AABB3::DoesRayIntersect( const Ray3& ray ) const {

	RaycastHit3 backResult = GetBackPlane().CheckForRayCollision(ray);
	RaycastHit3 frontResult = GetFrontPlane().CheckForRayCollision(ray);
	RaycastHit3 leftResult = GetLeftPlane().CheckForRayCollision(ray);
	RaycastHit3 rightResult = GetRightPlane().CheckForRayCollision(ray);
	RaycastHit3 topResult = GetTopPlane().CheckForRayCollision(ray);
	RaycastHit3 bottomResult = GetBottomPlane().CheckForRayCollision(ray);

	std::vector<RaycastHit3> hits;
	
	// if they are hits, add them to the result vector
	if (backResult.hit && Contains(backResult.position)) {
		hits.push_back(backResult);
	}
	if (frontResult.hit && Contains(frontResult.position)) {
		hits.push_back(frontResult);
	}
	if (leftResult.hit && Contains(leftResult.position)) {
		hits.push_back(leftResult);
	}
	if (rightResult.hit && Contains(rightResult.position)) {
		hits.push_back(rightResult);
	}
	if (topResult.hit && Contains(topResult.position)) {
		hits.push_back(topResult);
	}
	if (bottomResult.hit && Contains(bottomResult.position)) {
		hits.push_back(bottomResult);
	}

	// sort shortest distance first
	for (unsigned int hitIndex = 1; hitIndex < hits.size(); hitIndex++) {
		if (hits[hitIndex-1].distance > hits[hitIndex].distance) {
			RaycastHit3 temp = hits[hitIndex];
			hits[hitIndex] = hits[hitIndex-1];
			hits[hitIndex-1] = temp;
		}
	}

	return hits;
	

}


Plane AABB3::GetFrontPlane() const {
	Vector3 flb(mins.x, mins.y, maxs.z);
	Vector3 flt(mins.x, maxs.y, maxs.z);
	Vector3 frb(maxs.x, mins.y, maxs.z);
	return Plane(flb, flt, frb);
}

Plane AABB3::GetBackPlane() const {
	Vector3 blb(mins.x, mins.y, mins.z);
	Vector3 blt(mins.x, maxs.y, mins.z);
	Vector3 brb(maxs.x, mins.y, mins.z);
	return Plane(blb, blt, brb);
}

Plane AABB3::GetLeftPlane() const {
	Vector3 flb(mins.x, mins.y, maxs.z);
	Vector3 flt(mins.x, maxs.y, maxs.z);
	Vector3 blb(mins.x, mins.y, mins.z);
	return Plane(flb, flt, blb);
}

Plane AABB3::GetRightPlane() const {
	Vector3 frb(maxs.x, mins.y, maxs.z);
	Vector3 frt(maxs.x, maxs.y, maxs.z);
	Vector3 brb(maxs.x, mins.y, mins.z);
	return Plane(frb, frt, brb);
}

Plane AABB3::GetTopPlane() const {
	Vector3 flt(mins.x, maxs.y, maxs.z);
	Vector3 frt(maxs.x, maxs.y, maxs.z);
	Vector3 blt(mins.x, maxs.y, mins.z);
	return Plane(flt, flt, blt);
}

Plane AABB3::GetBottomPlane() const {
	Vector3 flb(mins.x, mins.y, maxs.z);
	Vector3 frb(maxs.x, mins.y, maxs.z);
	Vector3 blb(mins.x, mins.y, mins.z);
	return Plane(flb, flb, blb);
}


Vector3 AABB3::GetNearestPointOnSurface( const Vector3& outsidePoint ) {
	Vector3 result = outsidePoint;

	if ( outsidePoint.x < mins.x ) {
		result.x = mins.x;
	} else if ( outsidePoint.x > maxs.x ){
		result.x = maxs.x;
	}

	if ( outsidePoint.y < mins.y ) {
		result.y = mins.y;
	} else if ( outsidePoint.y > maxs.y ) {
		result.y = maxs.y;
	}

	if ( outsidePoint.z < mins.z ) {
		result.z = mins.z;
	} else if ( outsidePoint.z > maxs.z ) {
		result.z = maxs.z;
	}

	return result;
}


//----------------------------------------------------------------------------------------------------------------
bool AABB3::DoAABB3sOverlap( const AABB3& other ) const {
	if ( mins.x > other.maxs.x || maxs.x < other.mins.x ) {
		return false;
	}

	if ( mins.y > other.maxs.y || maxs.y < other.mins.y ) {
		return false;
	}

	if ( mins.z > other.maxs.z || maxs.z < other.mins.z ) {
		return false;
	}

	return true;
}