#include "Game/Jobs/TerrainRebuildJob.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"

//----------------------------------------------------------------------------------------------------------------
TerrainRebuildJob::TerrainRebuildJob( const Vector3& playerPosition, Terrain* terrain ) {

	m_centerVertexPosition.x = playerPosition.x - fmodf(playerPosition.x, 512.f);
	m_centerVertexPosition.y = 0.f;
	m_centerVertexPosition.z = playerPosition.z - fmodf(playerPosition.z, 512.f);

	m_terrain = terrain;
}


//----------------------------------------------------------------------------------------------------------------
TerrainRebuildJob::~TerrainRebuildJob() {

}


//----------------------------------------------------------------------------------------------------------------
void TerrainRebuildJob::Execute() {

	GenerateMeshWorking();
	//GenerateMeshBetterTangents();
}


//----------------------------------------------------------------------------------------------------------------
void TerrainRebuildJob::GenerateMeshWorking() {

	m_mb.Begin(TRIANGLES, false);

	float meshSize = 50000.f;
	int verticesOnSide = 300;
	float distPerVertex = meshSize / (float) verticesOnSide;

	m_mb.SetNormal(Vector3::UP);
	m_mb.SetTangent(Vector3::RIGHT);
	m_mb.SetColor(Rgba(255,255,255,255));

	// push back vertices in world coordinates
	for ( float z = m_centerVertexPosition.z - (meshSize * 0.5f); z <= m_centerVertexPosition.z + (meshSize * 0.5f); z += distPerVertex ) {
		for ( float x = m_centerVertexPosition.x - (meshSize * 0.5f); x <= m_centerVertexPosition.x + (meshSize * 0.5f); x += distPerVertex ) {

			float blHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(x, z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float brHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(x + distPerVertex, z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float tlHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(x, z + distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float trHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(x + distPerVertex, z + distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );


			Vector3 blPos = Vector3(x, blHeight, z);
			Vector3 brPos = Vector3(x + distPerVertex, brHeight, z);
			Vector3 trPos = Vector3(x + distPerVertex, trHeight, z + distPerVertex);
			Vector3 tlPos = Vector3(x, tlHeight, z + distPerVertex);

			Vector3 tangent = (brPos - blPos).GetNormalized();
			Vector3 bitangent = (tlPos - blPos).GetNormalized();
			Vector3 normal = Vector3::CrossProduct( tangent, bitangent );

			m_mb.SetNormal( normal );
			m_mb.SetTangent( tangent );
			m_mb.SetUV( Vector2( 0.f, 0.f ) );
			m_mb.PushVertex( blPos );
			m_mb.SetUV( Vector2( 1.f, 0.f ) );
			m_mb.PushVertex( brPos );
			m_mb.SetUV( Vector2( 1.f, 1.f ) );
			m_mb.PushVertex( trPos );

			m_mb.SetUV( Vector2( 0.f, 0.f ) );
			m_mb.PushVertex( blPos );
			m_mb.SetUV( Vector2( 1.f, 1.f ) );
			m_mb.PushVertex( trPos );
			m_mb.SetUV( Vector2( 0.f, 1.f ) );
			m_mb.PushVertex( tlPos );
		}

	}

	// Assign tangents
	for ( int index = 0; index < m_mb.GetVertexCount(); index++ ) {

		VertexMaster& vertex = m_mb.GetVertexByIndex( index );
		Vector3 pos = vertex.position;

		float southHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x,				 pos.z - distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
		float northHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x,				 pos.z + distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
		float westHeight  = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x + distPerVertex, pos.z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
		float eastHeight  = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x - distPerVertex, pos.z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );


		Vector3 southPos = Vector3(pos.x,				  southHeight, pos.z - distPerVertex);
		Vector3 northPos = Vector3(pos.x + distPerVertex, northHeight, pos.z + distPerVertex);
		Vector3 eastPos  = Vector3(pos.x - distPerVertex, eastHeight,  pos.z);
		Vector3 westPos  = Vector3(pos.x + distPerVertex, westHeight,  pos.z);

		Vector3 tangent = (eastPos - pos).GetNormalized();
		Vector3 bitangent = (northPos - pos).GetNormalized();
		Vector3 normal = Vector3::CrossProduct( tangent, bitangent );
		vertex.normal = normal;
		vertex.tangent = tangent;

	
	}

	// push back indices
	/*for ( int z = 0; z < verticesOnSide - 1; z++ ) {
	for ( int x = 0; x < verticesOnSide - 1; x++ ) {
	int bottomLeftIndex = (z * verticesOnSide) + x;
	int bottomRightIndex = bottomLeftIndex + 1;
	int topLeftIndex = bottomLeftIndex + verticesOnSide;
	int topRightIndex = topLeftIndex + 1;

	m_mb.PushIndex(bottomLeftIndex);
	m_mb.PushIndex(bottomRightIndex);
	m_mb.PushIndex(topRightIndex);
	m_mb.PushIndex(bottomLeftIndex);
	m_mb.PushIndex(topRightIndex);
	m_mb.PushIndex(topLeftIndex);
	}
	}*/

	// magic happens
	m_mb.End();
}


//----------------------------------------------------------------------------------------------------------------
void TerrainRebuildJob::GenerateMeshBetterTangents() {

	m_mb.Begin(TRIANGLES, true);

	float meshSize = m_terrain->meshLength;
	int verticesOnSide = m_terrain->verticesPerSide;
	float distPerVertex = meshSize / (float) verticesOnSide;

	m_mb.SetNormal(Vector3::UP);
	m_mb.SetTangent(Vector3::RIGHT);
	m_mb.SetColor(Rgba(255,255,255,255));

	bool zOdd = true;
	bool xOdd = true;

	// push back vertices in world coordinates
	for ( float z = m_centerVertexPosition.z - (meshSize * 0.5f); z < m_centerVertexPosition.z + (meshSize * 0.5f); z += distPerVertex ) {
		for ( float x = m_centerVertexPosition.x - (meshSize * 0.5f); x < m_centerVertexPosition.x + (meshSize * 0.5f); x += distPerVertex ) {

			float blHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(x, z, m_terrain->maxHeight, 3 ) ), -1.f, 1.f, 0.f, m_terrain->maxHeight );

			Vector3 blPos = Vector3(x, blHeight, z);

			m_mb.SetUV( Vector2( 0.5f, 0.5f ) );
			m_mb.PushVertex( blPos );
		}
	}

	// Assign tangents
	for ( int z = 0; z < verticesOnSide; z++ ) {
		for ( int x = 0; x < verticesOnSide; x++ ) {
			
			VertexMaster& vertex = m_mb.GetVertexByIndex( (z * verticesOnSide) + x );
			Vector3 pos = vertex.position;

			float southHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x,				 pos.z - distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float northHeight = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x,				 pos.z + distPerVertex, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float westHeight  = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x + distPerVertex, pos.z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );
			float eastHeight  = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise(pos.x - distPerVertex, pos.z, 3000.f, 3 ) ), -1.f, 1.f, 0.f, 3000.f );


			Vector3 southPos = Vector3(x,				  southHeight, z - distPerVertex);
			Vector3 northPos = Vector3(x + distPerVertex, northHeight, z + distPerVertex);
			Vector3 eastPos  = Vector3(x - distPerVertex, eastHeight,  z);
			Vector3 westPos  = Vector3(x + distPerVertex, westHeight,  z);

			Vector3 tangent = (eastPos - westPos).GetNormalized();
			Vector3 bitangent = (northPos - southPos).GetNormalized();
			Vector3 normal = Vector3::CrossProduct( bitangent, tangent );
			vertex.normal = normal;
			vertex.tangent = tangent;

		}
	}
	// push back indices
	for ( int z = 0; z < verticesOnSide - 1; z++ ) {
		for ( int x = 0; x < verticesOnSide - 1; x++ ) {
			int bottomLeftIndex = (z * verticesOnSide) + x;
			int bottomRightIndex = bottomLeftIndex + 1;
			int topLeftIndex = bottomLeftIndex + verticesOnSide;
			int topRightIndex = topLeftIndex + 1;
			
			m_mb.PushIndex(bottomLeftIndex);
			m_mb.PushIndex(bottomRightIndex);
			m_mb.PushIndex(topRightIndex);
			m_mb.PushIndex(bottomLeftIndex);
			m_mb.PushIndex(topRightIndex);
			m_mb.PushIndex(topLeftIndex);
		}
	}

	// magic happens
	m_mb.End();
}


//----------------------------------------------------------------------------------------------------------------
void TerrainRebuildJob::OnComplete() {
	Mesh* terrainMesh = new Mesh();
	terrainMesh->FromBuilderAsType<Vertex3D_Lit>(&m_mb);
	m_terrain->SetMesh( terrainMesh );
}