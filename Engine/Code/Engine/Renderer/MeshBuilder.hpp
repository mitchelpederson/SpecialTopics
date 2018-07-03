#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include <vector>


class MeshBuilder {

public:
	void Begin(DrawPrimitive primitive, bool useIndices);
	void End();
	void SetColor( const Rgba& color );
	void SetUV( const Vector2& uv );
	void SetNormal( const Vector3& normal );
	void SetTangent( const Vector3& tangent );
	unsigned int PushVertex( const Vector3& position );
	unsigned int PushQuad( const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl);

	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;
	const std::vector<VertexMaster>& GetVertices();
	const std::vector<unsigned int>& GetIndices();
	DrawInstructions GetDrawInstructions();


public:
	void BuildCube( Mesh* mesh, const Vector3& center, const Vector3& size, const Rgba& color = Rgba(255, 255, 255, 255), const AABB2& topUVs = AABB2::ZERO_TO_ONE, const AABB2& sideUVs = AABB2::ZERO_TO_ONE, const AABB2& bottomUVs = AABB2::ZERO_TO_ONE );
	void BuildWireCube( Mesh* mesh, const Vector3& center, const Vector3& size, const Rgba& color = Rgba(255, 255, 255, 255), const AABB2& topUVs = AABB2::ZERO_TO_ONE, const AABB2& sideUVs = AABB2::ZERO_TO_ONE, const AABB2& bottomUVs = AABB2::ZERO_TO_ONE );
	void BuildLine( Mesh* mesh, const Vector3& start, const Vector3& end, const Rgba& startColor, const Rgba& endColor );
	void BuildBasis( Mesh* mesh, const Matrix44& basis = Matrix44(), const Vector3& position = Vector3());
	void BuildQuad( Mesh* mesh, const Vector3& position, const Vector3& up, const Vector3& right, const Rgba& color);
	void BuildSphere( Mesh* mesh, const Vector3& position, float radius, unsigned int wedges, unsigned int slices, const Rgba& color = Rgba() ); 
	void BuildDeformedSphere( Mesh* mesh, const Vector3& position, float radius, unsigned int wedges, unsigned int slices, float deformAmount, const Rgba& color = Rgba() ); 
	void BuildTexturedGridFromPerlinParams( const IntVector2& facesInDimensions, const Vector2& faceDimensions, const Vector2& bottomLeftPosition, unsigned int seed, float perlinScale = 1.f, unsigned int perlinNumOctaves = 1, float perlinOctavePersistence = 0.5f, float perlinOctaveScale = 2.f );
	void BuildTexturedGridFromHeightMap( Image* heightMap, const std::vector<Vector3>& normals, const IntVector2& startIndex, unsigned int chunkSize, float minHeight, float maxHeight );
	void BuildTexturedGridFlat( unsigned int quadsPerDimension, float height );

	void BuildWireSphere( Mesh* mesh, const Vector3& position, float radius, unsigned int wedges, unsigned int slices, const Rgba& color = Rgba() );

	void AddCube( const Vector3& center, const Vector3& size, const Rgba& color = Rgba(255, 255, 255, 255), const AABB2& topUVs = AABB2::ZERO_TO_ONE, const AABB2& sideUVs = AABB2::ZERO_TO_ONE, const AABB2& bottomUVs = AABB2::ZERO_TO_ONE );
	void AddSphere( const Vector3& position, float radius, unsigned int wedges, unsigned int slices, const Rgba& color = Rgba() ); 


	void LoadMeshFromOBJ( const std::string& path );

private:
	VertexMaster m_stamp;
	std::vector<VertexMaster> m_vertices;
	std::vector<unsigned int> m_indices;
	DrawInstructions m_instructions;
};