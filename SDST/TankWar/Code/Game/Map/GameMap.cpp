#include "Game/Map/GameMap.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"

GameMap::GameMap() {
	GenerateHeightMap(GetRandomIntLessThan(1000000), MAP_CHUNKS_X, MAP_CHUNKS_Y, MAP_CHUNK_SIZE, MAP_MIN_HEIGHT, MAP_MAX_HEIGHT);
	GenerateNewChunks( MAP_CHUNKS_X, MAP_CHUNKS_Y );
}


GameMap::~GameMap() {
	for (unsigned int index = 0; index < m_chunks.size(); index++) {
		delete m_chunks[index];
		m_chunks[index] = nullptr;
	}
}


void GameMap::GenerateHeightMap( unsigned int seed, unsigned int chunksX, unsigned int chunksY, unsigned int chunkSize, float minHeight, float maxHeight ) {
	
	m_heightMap = new Image();
	m_heightMap->SetDimensions(chunksX * chunkSize + 1, chunksY * chunkSize + 1);
	m_normals.resize((chunksX * chunkSize + 1) * (chunksY * chunkSize + 1));
	m_dimensions = IntVector2(chunksX * chunkSize + 1, chunksY * chunkSize + 1);
	m_bounds = AABB3( Vector3(0.f, minHeight - 10.f, 0.f), Vector3(m_dimensions.x - 1, maxHeight + 10.f, m_dimensions.y - 1) );

	m_maxHeight = maxHeight;
	m_minHeight = minHeight;
	m_chunkSize = chunkSize;

	for (unsigned int row = 0; row < chunksY * chunkSize + 1; row++) {
		for (unsigned int col = 0; col < chunksX * chunkSize + 1; col++) {
			float noiseValue = Compute2dPerlinNoise( (float) col, (float) row, 60, 3, 0.5f, 2.f, true, seed );
			float mappedNoiseValue = RangeMapFloat(noiseValue, -1.f, 1.f, 0.f, 255.f);
			m_heightMap->PushTexel( Rgba(RoundToNearestInt(mappedNoiseValue), 0, 0, 0) );
		}
	}



	for (int normal = 0; normal < m_heightMap->GetDimensions().x * m_heightMap->GetDimensions().y; normal++) {
		IntVector2 coord = GetCoordForIndex(normal);
		

		/*

	    ul---u
		|\ 2|\
		| \ | \
		| 1\|3 \
		l---c---r
		 \ 6|\ 4| 
		  \ | \ | 
		   \|5 \|  
            d---dr
		*/

		int trianglesInvolved = 0;

		IntVector2 uCoord	= coord + IntVector2( 0, 1);
		IntVector2 dCoord	= coord + IntVector2( 0,-1);
		IntVector2 rCoord	= coord + IntVector2( 1, 0);
		IntVector2 lCoord	= coord + IntVector2(-1, 0);
		IntVector2 ulCoord	= coord + IntVector2(-1, 1);
		IntVector2 drCoord	= coord + IntVector2( 1,-1);

		float cHeight	= GetHeightAtDiscretePoint(coord);
		Vector3 c	= Vector3( coord.x, cHeight, coord.y); 

		Vector3 averageNormal = Vector3::ZERO;
		
		// Check if triangles 1 and 2 exist
		if (IsCoordValid(ulCoord)) {
			float uHeight	= GetHeightAtDiscretePoint(uCoord);
			float lHeight	= GetHeightAtDiscretePoint(lCoord);
			float ulHeight	= GetHeightAtDiscretePoint(ulCoord);
			Vector3 u	= Vector3( uCoord.x, uHeight, uCoord.y ); 
			Vector3 l	= Vector3( lCoord.x, lHeight, lCoord.y ); 
			Vector3 ul	= Vector3( ulCoord.x, ulHeight, ulCoord.y ); 
			
			Vector3 tangent1 = c - l;
			Vector3 bitangent1 = ul - l;
			Vector3 normal1 = Vector3::CrossProduct(bitangent1, tangent1);

			Vector3 tangent2 = u - c;
			Vector3 bitangent2 = u - ul;
			Vector3 normal2 = Vector3::CrossProduct(bitangent2, tangent2);
			
			trianglesInvolved += 2;
			averageNormal += normal1;
			averageNormal += normal2;
		}

		// check if triangle 3 exists
		if (IsCoordValid(uCoord) && IsCoordValid(rCoord)) {
			float uHeight	= GetHeightAtDiscretePoint(uCoord);
			float rHeight	= GetHeightAtDiscretePoint(rCoord);
			Vector3 u	= Vector3( uCoord.x, uHeight, uCoord.y ); 
			Vector3 r	= Vector3( rCoord.x, rHeight, rCoord.y ); 

			Vector3 tangent3 = r - c;
			Vector3 bitangent3 = u - c;
			Vector3 normal3 = Vector3::CrossProduct(bitangent3, tangent3);

			trianglesInvolved += 1;
			averageNormal += normal3;
		}

		// Check if triangles 4 and 5 exist
		if (IsCoordValid(drCoord)) {
			float dHeight	= GetHeightAtDiscretePoint(dCoord);
			float rHeight	= GetHeightAtDiscretePoint(rCoord);
			float drHeight	= GetHeightAtDiscretePoint(drCoord);
			Vector3 d	= Vector3( dCoord.x, dHeight, dCoord.y ); 
			Vector3 r	= Vector3( rCoord.x, rHeight, rCoord.y );
			Vector3 dr	= Vector3( drCoord.x, drHeight, drCoord.y );

			Vector3 tangent4 = r - c;
			Vector3 bitangent4 = r - dr;
			Vector3 normal4 = Vector3::CrossProduct(bitangent4, tangent4);

			Vector3 tangent5 = dr - d;
			Vector3 bitangent5 = c - d;
			Vector3 normal5 = Vector3::CrossProduct(bitangent5, tangent5);

			trianglesInvolved += 2;
			averageNormal += normal4;
			averageNormal += normal5;
		}

		// check if triangle 6 exists
		if (IsCoordValid(dCoord) && IsCoordValid(lCoord)) {
			float dHeight	= GetHeightAtDiscretePoint(dCoord);
			float lHeight	= GetHeightAtDiscretePoint(lCoord);
			Vector3 d	= Vector3( dCoord.x, dHeight, dCoord.y ); 
			Vector3 l	= Vector3( lCoord.x, lHeight, lCoord.y ); 

			Vector3 tangent6 = c - l;
			Vector3 bitangent6 = c - d;
			Vector3 normal6 = Vector3::CrossProduct(bitangent6, tangent6);

			trianglesInvolved += 1;
			averageNormal += normal6;
		}

		averageNormal = averageNormal * (1.f / (float) trianglesInvolved);

		m_normals[normal] = averageNormal.GetNormalized();
		
	}
}



void GameMap::GenerateNewChunks( unsigned int x, unsigned int y ) {
	int mapSeed = GetRandomIntInRange(0, 10000);

	Mesh* waterChunkMesh = new Mesh();
	MeshBuilder waterBuilder;
	waterBuilder.Begin(TRIANGLES, false);
	waterBuilder.BuildTexturedGridFlat( m_chunkSize, Interpolate( m_minHeight, m_maxHeight, 0.22f ) );
	waterBuilder.End();
	waterChunkMesh->FromBuilderAsType<Vertex3D_Lit>(&waterBuilder);

	for (int row = 0; row < y; row++) {
		for (int col = 0; col < x; col++) {

			// Terrain chunks
			Mesh* chunkMesh = new Mesh();
			MeshBuilder chunkBuilder;
			chunkBuilder.Begin(TRIANGLES, false);
			chunkBuilder.BuildTexturedGridFromHeightMap(m_heightMap, m_normals, IntVector2(m_chunkSize * col, m_chunkSize * row), m_chunkSize, m_minHeight, m_maxHeight );
			chunkBuilder.End();
			chunkMesh->FromBuilderAsType<Vertex3D_Lit>(&chunkBuilder);

			Renderable* chunk = new Renderable(chunkMesh, g_theRenderer->GetMaterial("terrain"));
			Transform transform;
			transform.position = Vector3( (float) (col * m_chunkSize), 0.f, (float) (row * m_chunkSize));
			chunk->SetModelMatrix(transform.GetLocalToWorldMatrix());
			m_chunks.push_back(chunk);
			g_theGame->GetCurrentPlayState()->m_scene->AddRenderable(chunk);


			// Water chunks
			Renderable* waterChunk = new Renderable(waterChunkMesh, g_theRenderer->GetMaterial("water"));
			waterChunk->SetModelMatrix(transform.GetLocalToWorldMatrix());
			m_waterChunks.push_back(waterChunk);
			g_theGame->GetCurrentPlayState()->m_scene->AddRenderable(waterChunk);

		}
	}
} 


float GameMap::GetHeightAtDiscretePoint( IntVector2 const& coord ) {
	return RangeMapFloat(m_heightMap->GetTexel(coord.x, coord.y).r, 0.f, 255.f, m_minHeight, m_maxHeight);
}


Vector3 GameMap::GetNormalAtDiscretePoint( IntVector2 const& coord ) {
	return m_normals[coord.y * m_dimensions.x + coord.x];
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


float GameMap::GetHeightAboveTerrainAtPoint( Vector3 const& point ) {
	float heightAtPoint = GetHeightAtPoint(Vector2(point.x, point.z));
	return point.y - heightAtPoint;
}


Vector3 GameMap::GetNormalAtPoint( Vector2 const& point ) {
	IntVector2 lowerCoord(point);
	IntVector2 upperCoord(point + Vector2(1.f, 1.f));
	float xRemainder = point.x - (float) lowerCoord.x;
	float yRemainder = point.y - (float) lowerCoord.y;

	Vector3 bottomLeftNormal	= GetNormalAtDiscretePoint( IntVector2( lowerCoord.x, lowerCoord.y ) );
	Vector3 bottomRightNormal	= GetNormalAtDiscretePoint( IntVector2( upperCoord.x, lowerCoord.y ) );
	Vector3 topLeftNormal		= GetNormalAtDiscretePoint( IntVector2( lowerCoord.x, upperCoord.y ) );
	Vector3 topRightNormal		= GetNormalAtDiscretePoint( IntVector2( upperCoord.x, upperCoord.y ) );

	Vector3 topInterpolated = Interpolate(topLeftNormal, topRightNormal, xRemainder);
	Vector3 bottomInterpolated = Interpolate(bottomLeftNormal, bottomRightNormal, xRemainder);
	Vector3 finalHeight = Interpolate(bottomInterpolated, topInterpolated, yRemainder);

	return finalHeight;
}


RaycastHit3 GameMap::Raycast( unsigned int maxContacts, const Ray3& ray ) {
	std::vector<RaycastHit3> boundsHits = m_bounds.DoesRayIntersect(ray);

	// If the ray hits bounds, then we should continue

	Vector3 abovePoint;
	Vector3 belowPoint;

	// If there was only one intersection with the bounds, then we start at the ray origin
	if ( boundsHits.size() <= 1 ) {
		abovePoint = ray.position;
	}
	else {
		abovePoint = boundsHits[0].position;
	}

	belowPoint = abovePoint + (ray.direction * 0.5f);

	// Step and sample until we get a point below the terrain. If we do 500,000 steps
	// then say we didn't hit anything
	unsigned int stepCount = 0;
	while(m_bounds.Contains(belowPoint) && GetHeightAboveTerrainAtPoint(belowPoint) > 0.f ) {
		if (stepCount == 500000) {
			// Early out if we never hit anything in a huge number of steps
			return RaycastHit3(false, ray.position + (ray.direction * 50000.f), 50000.f);
		} 
		abovePoint = belowPoint;
		belowPoint = abovePoint + (ray.direction * 0.5f);
		stepCount++;
	}

	if (!m_bounds.Contains(belowPoint)) {
		if (g_theGame->IsDevModeActive()) {
			DebugRenderPoint(1.f, belowPoint, Rgba(0, 0, 255, 255));
		}
		// Early out if the collision point is out of bounds of the game map
		return RaycastHit3(false, ray.position + (ray.direction * 50000.f), 50000.f);

	}


	// Now that we have a belowPoint actually below the terrain, close in on the exact
	// collision spot.
	Vector3 midpoint = Average(abovePoint, belowPoint);
	float terrainHeightAtMidpoint = GetHeightAboveTerrainAtPoint(midpoint);
	stepCount = 0;

	while (abs(terrainHeightAtMidpoint) > 0.0005f && stepCount < 10000) {
		if (terrainHeightAtMidpoint < 0.0f) {
			belowPoint = midpoint;
		}
		else {
			abovePoint = midpoint;
		}
		midpoint = Average(abovePoint, belowPoint);
		terrainHeightAtMidpoint = GetHeightAboveTerrainAtPoint(midpoint);
		stepCount++;
	}

	// debug render collision point
	if (g_theGame->IsDevModeActive()) {
		DebugRenderPoint(10.f, midpoint);
	}

	// debug render the ray of contact
	if ( g_theGame->IsDevModeActive()) {
		DebugRenderLineSegment(10.f, ray.position, Rgba(255, 0, 0, 255), midpoint, Rgba(255, 0, 0, 255));
	}

	// Now that we have closed in on our actual collision spot, lets return it
	return RaycastHit3(true, midpoint, (midpoint - ray.position).GetLength()); 
}



IntVector2 GameMap::GetDimensions() {
	return m_dimensions;
}

unsigned int GameMap::GetChunkSize() const {
	return m_chunkSize;
}


bool GameMap::IsCoordValid( IntVector2 const& coord ) {

	if (coord.x < 0 || coord.x >= m_dimensions.x) {
		return false;
	}
	if (coord.y < 0 || coord.y >= m_dimensions.y) {
		return false;
	}

	return true;
}

unsigned int GameMap::GetIndexForCoord( IntVector2 const& coord ) {
	return (coord.y * m_dimensions.x) + coord.x;
}

IntVector2 GameMap::GetCoordForIndex( unsigned int index ) {
	return IntVector2( index % m_dimensions.x, index / m_dimensions.x);
}