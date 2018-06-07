#pragma once
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Renderer/Renderable.h"
#include <vector>

class GameMap {
	
public:
	GameMap();
	~GameMap();

	float GetHeightAtPoint( Vector2 const& point );
	float GetHeightAtDiscretePoint( IntVector2 const& coord );

	IntVector2 GetDimensions();
	unsigned int GetChunkSize() const;

private:
	void GenerateHeightMap( unsigned int seed, unsigned int chunksX, unsigned int chunksY, unsigned int chunkSize, float minHeight, float maxHeight );
	void GenerateNewChunks( unsigned int x, unsigned int y );

private:
	float m_minHeight = 0.f;
	float m_maxHeight = 20.f;
	unsigned int m_chunkSize = 16;
	IntVector2 m_dimensions;

	std::vector<Renderable*> m_chunks;

	Image* m_heightMap;
};