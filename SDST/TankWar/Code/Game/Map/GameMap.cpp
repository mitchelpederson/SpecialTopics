#include "Game/Map/GameMap.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

GameMap::GameMap() {
	GenerateHeightMap(GetRandomIntLessThan(1000000), 16, 16, 16, 0.f, 20.f);
	GenerateNewChunks( 16, 16 );
}


GameMap::~GameMap() {
	for (unsigned int index = 0; index < m_chunks.size(); index++) {
		delete m_chunks[index];
		m_chunks[index] = nullptr;
	}
}


void GameMap::GenerateHeightMap( unsigned int seed, unsigned int chunksX, unsigned int chunksY, unsigned int chunkSize, float minHeight, float maxHeight ) {
	
	m_heightMap = new Image();
	m_heightMap->SetDimensions(chunksX * chunkSize, chunksY * chunkSize);

	m_maxHeight = maxHeight;
	m_minHeight = minHeight;
	m_chunkSize = chunkSize;
	for (unsigned int col = 0; col < chunksX * chunkSize; col++) {
		for (unsigned int row = 0; row < chunksY * chunkSize; row++) {
			float noiseValue = Compute2dPerlinNoise( (float) col, (float) row, 60, 3, 0.5f, 2.f, true, seed );
			float mappedNoiseValue = RangeMapFloat(noiseValue, -1.f, 1.f, 0.f, 255.f);
			m_heightMap->PushTexel( Rgba(RoundToNearestInt(mappedNoiseValue), 0, 0, 0) );
		}
	}
}



void GameMap::GenerateNewChunks( unsigned int x, unsigned int y ) {
	int mapSeed = GetRandomIntInRange(0, 10000);
	m_dimensions = IntVector2(x, y);

	for (int row = 0; row < m_dimensions.y - 1; row++) {
		for (int col = 0; col < m_dimensions.x - 1; col++) {

			Mesh* chunkMesh = new Mesh();
			MeshBuilder chunkBuilder;
			chunkBuilder.Begin(TRIANGLES, false);
			chunkBuilder.BuildTexturedGridFromHeightMap(m_heightMap, IntVector2(m_chunkSize * col, m_chunkSize * row), m_chunkSize, m_minHeight, m_maxHeight );
			chunkBuilder.End();
			chunkMesh->FromBuilderAsType<Vertex3D_Lit>(&chunkBuilder);

			Renderable* chunk = new Renderable(chunkMesh, g_theRenderer->GetMaterial("terrain"));
			Transform transform;
			transform.position = Vector3( (float) (col * 16), 0.f, (float) (row * 16));
			chunk->SetModelMatrix(transform.GetLocalToWorldMatrix());
			m_chunks.push_back(chunk);
			g_theGame->GetCurrentPlayState()->m_scene->AddRenderable(chunk);
		}
	}
} 


float GameMap::GetHeightAtDiscretePoint( IntVector2 const& coord ) {
	return RangeMapFloat(m_heightMap->GetTexel(coord.x, coord.y).r, 0.f, 255.f, m_minHeight, m_maxHeight);
}


float GameMap::GetHeightAtPoint( Vector2 const& point ) {
	IntVector2 lowerCoord(point);
	IntVector2 upperCoord(point + Vector2(1.f, 1.f));
	float xRemainder = point.x - (float) lowerCoord.x;
	float yRemainder = point.y - (float) lowerCoord.y;
	
	float bottomLeftHeight	= GetHeightAtDiscretePoint( IntVector2( lowerCoord.x, lowerCoord.y ) );
	float bottomRightHeight = GetHeightAtDiscretePoint( IntVector2( upperCoord.x, lowerCoord.y ) );
	float topLeftHeight		= GetHeightAtDiscretePoint( IntVector2( lowerCoord.x, upperCoord.y ) );
	float topRightHeight	= GetHeightAtDiscretePoint( IntVector2( upperCoord.x, upperCoord.y ) );

	float topInterpolated = Interpolate(topLeftHeight, topRightHeight, xRemainder);
	float bottomInterpolated = Interpolate(bottomLeftHeight, bottomRightHeight, xRemainder);
	float finalHeight = Interpolate(bottomInterpolated, topInterpolated, yRemainder);

	return finalHeight;

}


IntVector2 GameMap::GetDimensions() {
	return m_dimensions;
}

unsigned int GameMap::GetChunkSize() const {
	return m_chunkSize;
}