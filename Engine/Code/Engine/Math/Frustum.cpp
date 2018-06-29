#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vector4.hpp"

Frustum Frustum::FromMatrix( const Matrix44& mat ) {
	AABB3 ndc( Vector3(-1.f, -1.f, -1.f), Vector3(1.f, 1.f, 1.f) );

	Vector3 corners[8] = {
		Vector3(ndc.mins.x, ndc.mins.y, ndc.mins.z),
		Vector3(ndc.mins.x, ndc.mins.y, ndc.maxs.z),
		Vector3(ndc.mins.x, ndc.maxs.y, ndc.mins.z),
		Vector3(ndc.mins.x, ndc.maxs.y, ndc.maxs.z),
		Vector3(ndc.maxs.x, ndc.mins.y, ndc.mins.z),
		Vector3(ndc.maxs.x, ndc.mins.y, ndc.maxs.z),
		Vector3(ndc.maxs.x, ndc.maxs.y, ndc.mins.z),
		Vector3(ndc.maxs.x, ndc.maxs.y, ndc.maxs.z)
	};

	Matrix44 inverse = mat.GetInverse();
	Vector3 worldCorners[8];
	for (unsigned int i = 0; i < 8; i++) {
		Vector4 ndcPos = Vector4(corners[i], 1);
		Vector4 kindaWorldPos = inverse.Transform(ndcPos);

		worldCorners[i] = Vector3(kindaWorldPos.x, kindaWorldPos.y, kindaWorldPos.z) * (1.f / kindaWorldPos.w);
	}

	Frustum result;
	result.right = Plane(worldCorners[5], worldCorners[6], worldCorners[7]);
	result.left	 = Plane(worldCorners[0], worldCorners[1], worldCorners[2]);
	result.up	 = Plane(worldCorners[2], worldCorners[3], worldCorners[6]);
	result.down	 = Plane(worldCorners[0], worldCorners[1], worldCorners[4]);
	result.far   = Plane(worldCorners[1], worldCorners[3], worldCorners[5]);
	result.near  = Plane(worldCorners[0], worldCorners[2], worldCorners[4]);
	return result;
}

