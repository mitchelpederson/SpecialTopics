#pragma once
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Renderer/Renderable.h"
#include <vector>

class GameMap {
	
public:
	GameMap();
	~GameMap();

	float GetHeightAtPoint( Vector2 const& point );
	float GetHeightAtDiscretePoint( IntVector2 const& coord );
	float GetHeightAboveTerrainAtPoint( Vector3 const& point );
	Vector3 GetNormalAtDiscretePoint( IntVector2 const& coord );
	Vector3 GetNormalAtPoint( Vector2 const& point );
	RaycastHit3 Raycast( unsigned int maxContacts, const Ray3& ray );


	IntVector2 GetDimensions();
	unsigned int GetChunkSize() const;
	bool IsCoordValid( IntVector2 const& coord );

	unsigned int GetIndexForCoord( IntVector2 const& coord );
	IntVector2 GetCoordForIndex( unsigned int index );

private:
	void GenerateHeightMap( unsigned int seed, unsigned int chunksX, unsigned int chunksY, unsigned int chunkSize, float minHeight, float maxHeight );
	void GenerateNewChunks( unsigned int x, unsigned int y );

private:
	float m_minHeight;
	float m_maxHeight;
	AABB3 m_bounds; 
	unsigned int m_chunkSize = 16;
	IntVector2 m_dimensions;

	std::vector<Renderable*> m_chunks;
	std::vector<Renderable*> m_waterChunks;

	Image* m_heightMap;
	std::vector<Vector3> m_normals;
};