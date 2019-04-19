#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <string.h>
#include <fstream>


void MeshBuilder::Begin(DrawPrimitive primitive, bool useIndices) {
	if (m_indices.size() > 0) {
		m_indices.clear();
	}
	if (m_vertices.size() > 0) {
		m_vertices.clear();
	}
	m_instructions.type = primitive;
	m_instructions.useIndices = useIndices;

	if (useIndices) {
		m_instructions.startIndex = (unsigned int) m_indices.size();
	}
	else {
		m_instructions.startIndex = (unsigned int) m_vertices.size();
	}
}


void MeshBuilder::End() {
	unsigned int endIndex;
	if (m_instructions.useIndices) {
		endIndex = (unsigned int) m_indices.size();
		m_instructions.indexCount = endIndex - m_instructions.startIndex;
	} else {
		endIndex = (unsigned int) m_vertices.size();
		m_instructions.vertexCount = endIndex - m_instructions.startIndex;
	}
}


void MeshBuilder::SetColor( const Rgba& color ) {
	m_stamp.color = color;
}


void MeshBuilder::SetUV( const Vector2& uv ) {
	m_stamp.uv = uv;
}


void MeshBuilder::SetNormal( const Vector3& normal ) {
	m_stamp.normal = normal;
}

void MeshBuilder::SetTangent( const Vector3& tangent ) {
	m_stamp.tangent = tangent;
}


unsigned int MeshBuilder::PushVertex( const Vector3& position ) {
	m_stamp.position = position;
	m_vertices.push_back(m_stamp);
	return (unsigned int) m_vertices.size() - 1;
}


void MeshBuilder::PushIndex( unsigned int index ) {
	m_indices.push_back( index );
}


unsigned int MeshBuilder::PushQuad( const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl) {
	unsigned int firstIndex = (unsigned int) m_vertices.size();
	
	SetUV(Vector2(0.f, 0.f));
	PushVertex(bl);
	SetUV(Vector2(1.f, 0.f));
	PushVertex(br);
	SetUV(Vector2(1.f, 1.f));
	PushVertex(tr);
	SetUV(Vector2(0.f, 0.f));
	PushVertex(bl);
	SetUV(Vector2(1.f, 1.f));
	PushVertex(tr);
	SetUV(Vector2(0.f, 1.f));
	PushVertex(tl);

	return firstIndex;
}


unsigned int MeshBuilder::PushTexturedQuad( const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl, const AABB2& uvs ) {
	unsigned int firstIndex = (unsigned int) m_vertices.size();

	SetUV(uvs.mins);
	PushVertex(bl);
	SetUV(Vector2(uvs.maxs.x, uvs.mins.y));
	PushVertex(br);
	SetUV(uvs.maxs);
	PushVertex(tr);
	SetUV(uvs.mins);
	PushVertex(bl);
	SetUV(uvs.maxs);
	PushVertex(tr);
	SetUV(Vector2(uvs.mins.x, uvs.maxs.y));
	PushVertex(tl);

	return firstIndex;
}



unsigned int MeshBuilder::GetVertexCount() const {
	return (unsigned int) m_vertices.size() - m_instructions.startIndex;
}

unsigned int MeshBuilder::GetIndexCount() const {
	return (unsigned int) m_indices.size();
}

const std::vector<VertexMaster>& MeshBuilder::GetVertices() {
	return m_vertices;
}

const std::vector<unsigned int>& MeshBuilder::GetIndices() {
	return m_indices;
}


DrawInstructions MeshBuilder::GetDrawInstructions() {
	return m_instructions;
}


void MeshBuilder::LoadMeshFromOBJ( const std::string& path ) {
	std::ifstream objFile;
	objFile.open(path, std::ios::in);
	char* buffer = new char[256];

	std::vector<Vector3> vertPositions;
	std::vector<Vector3> vertNormals;
	std::vector<Vector2> vertUVs;


	Begin(TRIANGLES, true);
	while (!objFile.eof()) {
		objFile.getline(buffer, 256);
		std::vector<std::string> tokens = SplitString(buffer, ' ');

		if ( tokens.size() == 0 ) {
			continue;
		}

		if (tokens[0] == "v") {
			vertPositions.push_back( Vector3( -std::stof(tokens[1]), std::stof(tokens[2]), -std::stof(tokens[3]) ) );
		}

		else if (tokens[0] == "vn") {
			vertNormals.push_back( Vector3( -std::stof(tokens[1]), std::stof(tokens[2]), -std::stof(tokens[3]) ) );
		} 

		else if (tokens[0] == "vt") {
			vertUVs.push_back( Vector2( std::stof(tokens[1]), std::stof(tokens[2]) ) );
		}

		else if (tokens[0] == "f") {

			std::vector<unsigned int> faceIndices;
			for (unsigned int i = 1; i < tokens.size(); i++) {
				std::vector<std::string> faceTokens = SplitString(tokens[i], '/');
				int posIndex = 0;
				int uvIndex = 0;
				int normalIndex = 0;

				if (faceTokens.size() == 2) {
					posIndex = std::stoi(faceTokens[0]) - 1;
					normalIndex = std::stoi(faceTokens[1]) - 1;
				}

				else if ( faceTokens.size() == 3 ) {
					posIndex = std::stoi(faceTokens[0]) - 1;
					if ( faceTokens[1] != "" ) {
						uvIndex = std::stoi(faceTokens[1]) - 1;
					} 
					normalIndex = std::stoi(faceTokens[2]) - 1;
					
					SetNormal(vertNormals[normalIndex]);
				}

				SetUV(vertUVs[uvIndex]);
				unsigned int index = PushVertex(vertPositions[posIndex]);
				faceIndices.push_back(index);
			}

				// Triangle
			if (faceIndices.size() == 3) {
				/*VertexMaster& a = m_vertices[faceIndices[0]];
				VertexMaster& b = m_vertices[faceIndices[1]];
				VertexMaster& c = m_vertices[faceIndices[2]];
				*/

				m_indices.push_back(faceIndices[0]);
				m_indices.push_back(faceIndices[2]);
				m_indices.push_back(faceIndices[1]);
			}

				// Quad
			else if (faceIndices.size() == 4) {
				m_indices.push_back(faceIndices[0]);
				m_indices.push_back(faceIndices[2]);
				m_indices.push_back(faceIndices[1]);
				m_indices.push_back(faceIndices[0]);
				m_indices.push_back(faceIndices[3]);
				m_indices.push_back(faceIndices[2]);
			}
		}
	}
	End();

	delete buffer;
	buffer = nullptr;
}



//-------------------------------------------------------------------
void MeshBuilder::BuildCube(Mesh* mesh
	, const Vector3& center
	, const Vector3& size
	, const Rgba& color /* = Rgba(255, 255, 255, 255) */
	, const AABB2& topUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& sideUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& bottomUVs /* = Vector2::ZERO_TO_ONE */) {

	Begin(TRIANGLES, false);
	SetColor(color);

	Vector3 halfSize = size * 0.5f;

	float left = center.x - halfSize.x;
	float right = center.x + halfSize.x;
	float top = center.y + halfSize.y;
	float bottom = center.y - halfSize.y;
	float front = center.z + halfSize.z;
	float back = center.z - halfSize.z;

	Vector3 leftTopFront(left, top, front);
	Vector3 rightTopFront(right, top, front);
	Vector3 leftBottomFront(left, bottom, front);
	Vector3 rightBottomFront(right, bottom, front);
	Vector3 leftTopBack(left, top, back);
	Vector3 rightTopBack(right, top, back);
	Vector3 leftBottomBack(left, bottom, back);
	Vector3 rightBottomBack(right, bottom, back);

	// front face
	SetNormal(Vector3::FORWARD);
	SetTangent(Vector3::RIGHT);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomFront);

	// back face
	SetNormal(Vector3::FORWARD * -1.f);
	SetTangent(Vector3::RIGHT * -1.f);
	SetUV( Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex( rightTopBack );
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex( leftTopBack );
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex( rightBottomBack );
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex( leftTopBack );
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex( leftBottomBack );
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex( rightBottomBack );

	// left face
	SetNormal(Vector3::RIGHT * -1.f);
	SetTangent(Vector3::FORWARD * -1.f);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(leftTopBack);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomBack);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(leftBottomFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomBack);

	// right face
	SetNormal(Vector3::RIGHT);
	SetTangent(Vector3::FORWARD);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(rightBottomBack);	
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(rightBottomFront);

	// top face
	SetNormal(Vector3::UP);
	SetTangent(Vector3::RIGHT);
	SetUV(Vector2( topUVs.mins.x, topUVs.maxs.y ));
	PushVertex(leftTopBack);
	SetUV(Vector2( topUVs.maxs.x, topUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( topUVs.mins.x, topUVs.mins.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( topUVs.maxs.x, topUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( topUVs.maxs.x, topUVs.mins.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( topUVs.mins.x, topUVs.mins.y ));
	PushVertex(leftTopFront);

	// bottom face
	SetNormal(Vector3::UP * -1.f);
	SetTangent(Vector3::RIGHT * -1.f);
	SetUV(Vector2( bottomUVs.mins.x, bottomUVs.maxs.y ));
	PushVertex(leftBottomFront);
	SetUV(Vector2( bottomUVs.maxs.x, bottomUVs.maxs.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( bottomUVs.mins.x, bottomUVs.mins.y ));
	PushVertex(leftBottomBack);
	SetUV(Vector2( bottomUVs.maxs.x, bottomUVs.maxs.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( bottomUVs.maxs.x, bottomUVs.mins.y ));
	PushVertex(rightBottomBack);
	SetUV(Vector2( bottomUVs.mins.x, bottomUVs.mins.y ));
	PushVertex(leftBottomBack);

	End();
	//mesh->SetMesh(36, vertices);
	mesh->FromBuilderAsType<Vertex3D_Lit>(this);
}


void MeshBuilder::BuildWireCube(Mesh* mesh
	, const Vector3& center
	, const Vector3& size
	, const Rgba& color /* = Rgba(255, 255, 255, 255) */
	, const AABB2& topUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& sideUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& bottomUVs /* = Vector2::ZERO_TO_ONE */) {

	Begin(TRIANGLES, false);
	Vertex3D_PCU vertices[48];

	Vector3 halfSize = size * 0.5f;

	float left = center.x - halfSize.x;
	float right = center.x + halfSize.x;
	float top = center.y + halfSize.y;
	float bottom = center.y - halfSize.y;
	float front = center.z - halfSize.z;
	float back = center.z + halfSize.z;

	Vector3 leftTopFront(left, top, front);
	Vector3 rightTopFront(right, top, front);
	Vector3 leftBottomFront(left, bottom, front);
	Vector3 rightBottomFront(right, bottom, front);
	Vector3 leftTopBack(left, top, back);
	Vector3 rightTopBack(right, top, back);
	Vector3 leftBottomBack(left, bottom, back);
	Vector3 rightBottomBack(right, bottom, back);
	SetColor(color);
	
	// front face
	PushVertex(leftTopFront);
	PushVertex(rightTopFront);
	PushVertex(rightTopFront);
	PushVertex(rightBottomFront);
	PushVertex(rightBottomFront);
	PushVertex(leftBottomFront);
	PushVertex(leftBottomFront);
	PushVertex(leftTopFront);


	// back face
	vertices[8].position = leftTopBack;
	vertices[9].position = rightTopBack;
	vertices[10].position = rightTopBack;
	vertices[11].position = rightBottomBack;
	vertices[12].position = rightBottomBack;
	vertices[13].position = leftBottomBack;
	vertices[14].position = leftBottomBack;
	vertices[15].position = leftTopBack;

	// left face
	vertices[16].position = leftTopBack;
	vertices[17].position = leftTopFront;
	vertices[18].position = leftTopFront;
	vertices[19].position = leftBottomFront;
	vertices[20].position = leftBottomFront;
	vertices[21].position = leftBottomBack;
	vertices[22].position = leftBottomBack;
	vertices[23].position = leftTopBack;

	// right face
	vertices[24].position = rightTopBack;
	vertices[25].position = rightTopFront;
	vertices[26].position = rightTopFront;
	vertices[27].position = rightBottomFront;
	vertices[28].position = rightBottomFront;
	vertices[29].position = rightBottomBack;
	vertices[30].position = rightBottomBack;
	vertices[31].position = rightTopBack;

	// top face
	vertices[32].position = leftTopBack;
	vertices[33].position = rightTopBack;
	vertices[34].position = rightTopBack;
	vertices[35].position = rightTopFront;
	vertices[36].position = rightTopFront;
	vertices[37].position = leftTopFront;
	vertices[38].position = leftTopFront;
	vertices[39].position = leftTopBack;


	// bottom face
	vertices[40].position = leftBottomBack;
	vertices[41].position = rightBottomBack;
	vertices[42].position = rightBottomBack;
	vertices[43].position = rightBottomFront;
	vertices[44].position = rightBottomFront;
	vertices[45].position = leftBottomFront;
	vertices[46].position = leftBottomFront;
	vertices[47].position = leftBottomBack;

	for (int i = 0; i < 48; i++) {
		vertices[i].color = color;
	}
	mesh->SetMesh(48, vertices);
	mesh->SetDrawPrimitive(LINES);
}



void MeshBuilder::BuildQuad( Mesh* mesh, const Vector3& position, const Vector3& up, const Vector3& right, const Rgba& color ) {
	Vertex3D_PCU vertices[6];

	Begin(TRIANGLES, false);

	SetColor(color);
	SetNormal( Vector3::CrossProduct(up, right) );
	SetTangent( right );

	SetUV( Vector2( 0.f, 0.f ) );
	PushVertex( position );

	SetUV( Vector2( 1.f, 1.f ) );
	PushVertex( position + up + right );

	SetUV( Vector2( 0.f, 1.f ) );
	PushVertex( position + up );

	SetUV( Vector2( 0.f, 0.f ) );
	PushVertex( position );

	SetUV( Vector2( 1.f, 0.f ) );
	PushVertex( position + right );

	SetUV( Vector2( 1.f, 1.f ) );
	PushVertex( position + up + right );

	End();

	mesh->FromBuilderAsType<Vertex3D_Lit>(this);
}


void MeshBuilder::BuildLine( Mesh* mesh, const Vector3& start, const Vector3& end, const Rgba& startColor, const Rgba& endColor ) {
	Vertex3D_PCU vertices[2];

	vertices[0].position = start;
	vertices[0].uv = Vector2();
	vertices[0].color = startColor;
	vertices[1].position = end;
	vertices[1].uv = Vector2();
	vertices[1].color = endColor;

	mesh->SetMesh(2, vertices);
}

void MeshBuilder::BuildBasis( Mesh* mesh, const Matrix44& basis, const Vector3& position ) {

	Vertex3D_PCU vertices[6];

	Vector3 iBasis = basis.GetI();
	Vector3 jBasis = basis.GetJ();
	Vector3 kBasis = basis.GetK();

	vertices[0].position = position;
	vertices[0].uv = Vector2();
	vertices[0].color = Rgba(255, 0, 0, 255);
	vertices[1].position = iBasis + position;
	vertices[1].uv = Vector2();
	vertices[1].color = Rgba(255, 0, 0, 255);

	vertices[2].position = position;
	vertices[2].uv = Vector2();
	vertices[2].color = Rgba(0, 255, 0, 255);
	vertices[3].position = jBasis + position;
	vertices[3].uv = Vector2();
	vertices[3].color = Rgba(0, 255, 0, 255);

	vertices[4].position = position;
	vertices[4].uv = Vector2();
	vertices[4].color = Rgba(0, 120, 255, 255);
	vertices[5].position = kBasis + position;
	vertices[5].uv = Vector2();
	vertices[5].color = Rgba(0, 120, 255, 255);

	mesh->SetMesh(6, vertices);
	mesh->SetDrawPrimitive(LINES);

}


void MeshBuilder::BuildSphere( Mesh* mesh
	, const Vector3& position
	, float radius
	, unsigned int wedges
	, unsigned int slices
	, const Rgba& color /* = Rgba() */ ) {

	Begin(TRIANGLES, true);

	for (unsigned int slice = 0; slice <= slices; slice++) {
		float v = (float) slice / (float) slices;
		float verticalDegrees = RangeMapFloat(v, 0.f, 1.f, -90.f, 90.f);

		for (unsigned int wedge = 0; wedge <= wedges; wedge++) {
			float u = (float) wedge / (float) wedges;
			float horizontalDegrees = u * 360.f;

			Vector2 uv(u, v);
			Vector3 vertPos = position + PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees);
			SetNormal(PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees));
			SetUV(uv);
			PushVertex(vertPos);
		}
	}

	for (unsigned int wedgeIndex = 0; wedgeIndex < wedges; ++wedgeIndex) {
		for (unsigned int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
			unsigned int bottomLeft = (sliceIndex * (wedges + 1)) + wedgeIndex;
			unsigned int bottomRight = bottomLeft + 1;
			unsigned int topLeft = bottomLeft + wedges + 1;
			unsigned int topRight = topLeft + 1;

			Vector3 bottomRightVertex = m_vertices[bottomRight].position;
			Vector3 bottomLeftVertex = m_vertices[bottomLeft].position;
			Vector3 displacement = bottomRightVertex - bottomLeftVertex;
			Vector3 tangent = displacement.GetNormalized();

			m_vertices[bottomRight].tangent = tangent;
			m_vertices[bottomLeft].tangent = tangent;
			m_vertices[topRight].tangent = tangent;
			m_vertices[topLeft].tangent = tangent;

			m_indices.push_back(bottomLeft);
			m_indices.push_back(bottomRight);
			m_indices.push_back(topRight);
			m_indices.push_back(bottomLeft);
			m_indices.push_back(topRight);
			m_indices.push_back(topLeft);
		}
	}
	End();

	mesh->FromBuilderAsType<Vertex3D_Lit>( this );
	//mesh->SetMesh((unsigned int) vertices.size(), (unsigned int) indices.size(), vertices.data(), indices.data());
}


void MeshBuilder::BuildDeformedSphere( Mesh* mesh
	, const Vector3& position
	, float radius
	, unsigned int wedges
	, unsigned int slices
	, float deformFactor
	, const Rgba& color /* = Rgba() */ ) {

	Begin(TRIANGLES, true);

	for (unsigned int slice = 0; slice <= slices; slice++) {
		float v = (float) slice / (float) slices;
		float verticalDegrees = RangeMapFloat(v, 0.f, 1.f, -90.f, 90.f);

		for (unsigned int wedge = 0; wedge <= wedges; wedge++) {
			float u = (float) wedge / (float) wedges;
			float horizontalDegrees = u * 360.f;

			
			Vector3 vertCoords = PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees);
			float noise = GetRandomFloatInRange(-deformFactor * 0.5f, deformFactor * 0.5f);

			Vector2 uv(u, v);
			Vector3 vertPos = position + vertCoords + (vertCoords.GetNormalized() * noise);
			SetNormal(vertCoords.GetNormalized());
			SetUV(uv);
			PushVertex(vertPos);
		}
	}

	// We need to close the seam
	for (unsigned int sliceIndex = 0; sliceIndex < slices; sliceIndex++) {
		unsigned int leftVertexIndex = sliceIndex * (wedges + 1);
		unsigned int rightVertexIndex = (sliceIndex * (wedges + 1)) + wedges;

		m_vertices[rightVertexIndex].position = m_vertices[leftVertexIndex].position;
	}

	for (unsigned int wedgeIndex = 0; wedgeIndex < wedges; ++wedgeIndex) {
		for (unsigned int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
			unsigned int bottomLeft = (sliceIndex * (wedges + 1)) + wedgeIndex;
			unsigned int bottomRight = bottomLeft + 1;
			unsigned int topLeft = bottomLeft + wedges + 1;
			unsigned int topRight = topLeft + 1;

			Vector3 bottomRightVertex = m_vertices[bottomRight].position;
			Vector3 bottomLeftVertex = m_vertices[bottomLeft].position;
			Vector3 displacement = bottomRightVertex - bottomLeftVertex;

			Vector3 tangent = displacement.GetNormalized();

			m_vertices[bottomRight].tangent = tangent;
			m_vertices[bottomLeft].tangent = tangent;
			m_vertices[topRight].tangent = tangent;
			m_vertices[topLeft].tangent = tangent;

			m_indices.push_back(bottomLeft);
			m_indices.push_back(bottomRight);
			m_indices.push_back(topRight);
			m_indices.push_back(bottomLeft);
			m_indices.push_back(topRight);
			m_indices.push_back(topLeft);
		}
	}
	End();

	mesh->FromBuilderAsType<Vertex3D_Lit>( this );
	//mesh->SetMesh((unsigned int) vertices.size(), (unsigned int) indices.size(), vertices.data(), indices.data());
}

void MeshBuilder::BuildWireSphere( Mesh* mesh
	, const Vector3& position
	, float radius
	, unsigned int wedges
	, unsigned int slices
	, const Rgba& color /* = Rgba() */ ) {

	//std::vector<Vertex3D_Lit> vertices;
	//std::vector<unsigned int> indices;
	Begin(LINES, true);
	SetColor(color);

	for (unsigned int slice = 0; slice <= slices; slice++) {
		float v = (float) slice / (float) slices;
		float verticalDegrees = RangeMapFloat(v, 0.f, 1.f, -90.f, 90.f);

		for (unsigned int wedge = 0; wedge <= wedges; wedge++) {
			float u = (float) wedge / (float) wedges;
			float horizontalDegrees = u * 360.f;

			SetUV(Vector2(u, v));
			Vector3 vertPos = position + PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees);
			SetNormal(PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees).GetNormalized());
			PushVertex(vertPos);
		}
	}

	for (unsigned int wedgeIndex = 0; wedgeIndex < wedges; ++wedgeIndex) {
		for (unsigned int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
			unsigned int bottomLeft = (sliceIndex * (wedges + 1)) + wedgeIndex;
			unsigned int bottomRight = bottomLeft + 1;
			unsigned int topLeft = bottomLeft + wedges + 1;
			unsigned int topRight = topLeft + 1;

			m_indices.push_back(bottomLeft);
			m_indices.push_back(bottomRight);
			m_indices.push_back(bottomRight);
			m_indices.push_back(topRight);
			m_indices.push_back(topRight);
			m_indices.push_back(topLeft);		
			m_indices.push_back(topLeft);			
			m_indices.push_back(bottomLeft);

		}
	}

	End();

	mesh->FromBuilderAsType<Vertex3D_Lit>(this);
	mesh->SetDrawPrimitive(LINES);
}


void MeshBuilder::BuildTexturedGridFromPerlinParams( const IntVector2& facesInDimensions
												   , const Vector2& faceDimensions
												   , const Vector2& bottomLeftPosition
												   , unsigned int seed
												   , float perlinScale
												   , unsigned int perlinNumOctaves
												   , float perlinOctavePersistence
												   , float perlinOctaveScale ) {

	SetColor(Rgba());

	for (int row = 0; row < facesInDimensions.x; row++) {
		for (int col = 0; col < facesInDimensions.y; col++) {
			Vector3 bottomLeft	( (float) col,						0.f, (float) row );
			Vector3 bottomRight	( (float) faceDimensions.x + col,	0.f, (float) row );
			Vector3 topLeft		( (float) col,						0.f, (float) row + faceDimensions.y );
			Vector3 topRight	( (float) faceDimensions.x + col,	0.f, (float) row + faceDimensions.y );
			bottomLeft.y	= 10.f * Compute2dPerlinNoise( bottomLeft.x + bottomLeftPosition.x, bottomLeft.z + bottomLeftPosition.y, perlinScale, perlinNumOctaves, perlinOctavePersistence, perlinOctaveScale, true, seed );
			bottomRight.y	= 10.f * Compute2dPerlinNoise( bottomRight.x + bottomLeftPosition.x, bottomRight.z + bottomLeftPosition.y, perlinScale, perlinNumOctaves, perlinOctavePersistence, perlinOctaveScale, true, seed );
			topLeft.y		= 10.f * Compute2dPerlinNoise( topLeft.x + bottomLeftPosition.x, topLeft.z + bottomLeftPosition.y, perlinScale, perlinNumOctaves, perlinOctavePersistence, perlinOctaveScale, true, seed );
			topRight.y		= 10.f * Compute2dPerlinNoise( topRight.x + bottomLeftPosition.x, topRight.z + bottomLeftPosition.y, perlinScale, perlinNumOctaves, perlinOctavePersistence, perlinOctaveScale, true, seed );
			
			Vector3 bottomTangent = (bottomRight - bottomLeft).GetNormalized();
			Vector3 bottomBitangent = (topLeft - bottomLeft).GetNormalized();
			Vector3 bottomNormal = Vector3::CrossProduct(bottomBitangent, bottomTangent).GetNormalized();
			Vector3 topTangent = (topRight - topLeft).GetNormalized();
			Vector3 topBitangent = (topRight - bottomRight).GetNormalized();
			Vector3 topNormal = Vector3::CrossProduct(topBitangent, topTangent).GetNormalized();

/*			Vector2 bottomLeftUV  ( (float) col * uIncrement,		(float) row * vIncrement );
			Vector2 bottomRightUV ( (float) (col + 1) * uIncrement, (float) row * vIncrement );
			Vector2 topLeftUV	  ( (float) col * uIncrement,		(float) (row + 1) * vIncrement );
			Vector2 topRightUV    ( (float) (col + 1) * uIncrement, (float) (row + 1) * vIncrement );
*/

			Vector2 bottomLeftUV  ( 0.f, 0.f );
			Vector2 bottomRightUV ( 1.f, 0.f );
			Vector2 topLeftUV	  ( 0.f, 1.f );
			Vector2 topRightUV    ( 1.f, 1.f );

			SetNormal(bottomNormal);
			SetTangent(bottomTangent);

			SetUV(bottomLeftUV);
			PushVertex(bottomLeft);
			SetUV(topLeftUV);
			PushVertex(topLeft);
			SetUV(bottomRightUV);
			PushVertex(bottomRight);

			SetNormal(topNormal);
			SetTangent(topTangent);

			SetUV(bottomRightUV);
			PushVertex(bottomRight);
			SetUV(topLeftUV);
			PushVertex(topLeft);
			SetUV(topRightUV);
			PushVertex(topRight);
		}
	}
}


void MeshBuilder::BuildTexturedGridFromHeightMap( Image* heightMap, const std::vector<Vector3>& normals, const IntVector2& startIndex, unsigned int chunkSize, float minHeight, float maxHeight ) {

	SetColor(Rgba());

	IntVector2 endIndex( startIndex.x + chunkSize, startIndex.y + chunkSize );

	for ( unsigned int row = 0; row < chunkSize; row++) {
		for ( unsigned int col = 0; col < chunkSize; col++) {
			Vector3 bottomLeft	( (float) col,		0.f, (float) row );
			Vector3 bottomRight	( (float) 1 + col,	0.f, (float) row );
			Vector3 topLeft		( (float) col,		0.f, (float) row + 1 );
			Vector3 topRight	( (float) 1 + col,	0.f, (float) row + 1 );
			bottomLeft.y	= RangeMapFloat(heightMap->GetTexel(startIndex.x + col,		startIndex.y + row	).r, 0.f, 255.f, minHeight, maxHeight);
			bottomRight.y	= RangeMapFloat(heightMap->GetTexel(startIndex.x + col + 1, startIndex.y + row	).r, 0.f, 255.f, minHeight, maxHeight);
			topLeft.y		= RangeMapFloat(heightMap->GetTexel(startIndex.x + col,		startIndex.y + row + 1).r, 0.f, 255.f, minHeight, maxHeight);
			topRight.y		= RangeMapFloat(heightMap->GetTexel(startIndex.x + col + 1, startIndex.y + row + 1).r, 0.f, 255.f, minHeight, maxHeight);

			// Get normals from the passed in normal map
			unsigned int bottomLeftIndex = (startIndex.y + row) * heightMap->GetDimensions().x + (startIndex.x + col);
			unsigned int bottomRightIndex = (startIndex.y + row) * heightMap->GetDimensions().x + (startIndex.x + col + 1);
			unsigned int topLeftIndex = (startIndex.y + row + 1) * heightMap->GetDimensions().x + (startIndex.x + col);
			unsigned int topRightIndex = (startIndex.y + row + 1) * heightMap->GetDimensions().x + (startIndex.x + col + 1);
			Vector3 bottomLeftNormal( normals[bottomLeftIndex] );
			Vector3 bottomRightNormal( normals[bottomRightIndex] );
			Vector3 topLeftNormal( normals[topLeftIndex] );
			Vector3 topRightNormal( normals[topRightIndex] );

			// Generate tangents using that normal and the bitangent
			Vector3 bottomBitangent = (topLeft - bottomLeft).GetNormalized();
			Vector3 topBitangent = (topRight - bottomRight).GetNormalized();
			Vector3 bottomLeftTangent = Vector3::CrossProduct(bottomLeftNormal, bottomBitangent);
			Vector3 bottomRightTangent = Vector3::CrossProduct(bottomRightNormal, topBitangent);
			Vector3 topLeftTangent = Vector3::CrossProduct(topLeftNormal, bottomBitangent);
			Vector3 topRightTangent = Vector3::CrossProduct(topRightNormal, topBitangent);

			Vector2 bottomLeftUV  ( 0.f, 0.f );
			Vector2 bottomRightUV ( 1.f, 0.f );
			Vector2 topLeftUV	  ( 0.f, 1.f );
			Vector2 topRightUV    ( 1.f, 1.f );

			SetNormal(bottomLeftNormal);
			SetTangent(bottomLeftTangent);
			SetUV(bottomLeftUV);
			PushVertex(bottomLeft);

			SetNormal(bottomRightNormal);
			SetTangent(bottomRightTangent);
			SetUV(bottomRightUV);
			PushVertex(bottomRight);

			SetNormal(topLeftNormal);
			SetTangent(topLeftTangent);
			SetUV(topLeftUV);
			PushVertex(topLeft);

			SetNormal(bottomRightNormal);
			SetTangent(bottomRightTangent);
			SetUV(bottomRightUV);
			PushVertex(bottomRight);

			SetNormal(topRightNormal);
			SetTangent(topRightTangent);
			SetUV(topRightUV);
			PushVertex(topRight);

			SetNormal(topLeftNormal);
			SetTangent(topLeftTangent);
			SetUV(topLeftUV);
			PushVertex(topLeft);
			
		}
	}
}


void MeshBuilder::BuildTexturedGridFlat( unsigned int chunkSize, float height ) {

	SetColor(Rgba());

	for ( unsigned int row = 0; row < chunkSize; row++) {
		for ( unsigned int col = 0; col < chunkSize; col++) {
			Vector3 bottomLeft	( (float) col,		height, (float) row );
			Vector3 bottomRight	( (float) 1 + col,	height, (float) row );
			Vector3 topLeft		( (float) col,		height, (float) row + 1 );
			Vector3 topRight	( (float) 1 + col,	height, (float) row + 1 );

			Vector2 bottomLeftUV  ( 0.f, 0.f );
			Vector2 bottomRightUV ( 1.f, 0.f );
			Vector2 topLeftUV	  ( 0.f, 1.f );
			Vector2 topRightUV    ( 1.f, 1.f );

			SetNormal(Vector3::UP);
			SetTangent(Vector3::RIGHT);
			SetUV(bottomLeftUV);
			PushVertex(bottomLeft);
			SetUV(bottomRightUV);
			PushVertex(bottomRight);
			SetUV(topLeftUV);
			PushVertex(topLeft);
			SetUV(bottomRightUV);
			PushVertex(bottomRight);
			SetUV(topRightUV);
			PushVertex(topRight);
			SetUV(topLeftUV);
			PushVertex(topLeft);

		}
	}
}



//-------------------------------------------------------------------
void MeshBuilder::AddCube(
	  const Vector3& center
	, const Vector3& size
	, const Rgba& color /* = Rgba(255, 255, 255, 255) */
	, const AABB2& topUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& sideUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& bottomUVs /* = Vector2::ZERO_TO_ONE */) {

	unsigned int vertices = (unsigned int) m_vertices.size();
	SetColor(color);

	Vector3 halfSize = size * 0.5f;

#ifdef X_RIGHT_Y_UP_Z_FORWARD
	float left = center.x - halfSize.x;
	float right = center.x + halfSize.x;
	float top = center.y + halfSize.y;
	float bottom = center.y - halfSize.y;
	float front = center.z + halfSize.z;
	float back = center.z - halfSize.z;
	Vector3 leftTopFront(left, top, front);
	Vector3 rightTopFront(right, top, front);
	Vector3 leftBottomFront(left, bottom, front);
	Vector3 rightBottomFront(right, bottom, front);
	Vector3 leftTopBack(left, top, back);
	Vector3 rightTopBack(right, top, back);
	Vector3 leftBottomBack(left, bottom, back);
	Vector3 rightBottomBack(right, bottom, back);
#endif

#ifdef X_FORWARD_Y_LEFT_Z_UP
	float left = center.y + halfSize.y;
	float right = center.y - halfSize.y;
	float top = center.z + halfSize.z;
	float bottom = center.z - halfSize.z;
	float front = center.x + halfSize.x;
	float back = center.x - halfSize.x;
	Vector3 leftTopFront(front, left, top);
	Vector3 rightTopFront(front, right, top);
	Vector3 leftBottomFront(front, left, bottom);
	Vector3 rightBottomFront(front, right, bottom);
	Vector3 leftTopBack(back, left, top);
	Vector3 rightTopBack(back, right, top);
	Vector3 leftBottomBack(back, left, bottom);
	Vector3 rightBottomBack(back, right, bottom);
#endif


	// front face
	SetNormal(Vector3::FORWARD);
	SetTangent(Vector3::RIGHT);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(rightBottomFront);
	m_indices.push_back(vertices + 0);
	m_indices.push_back(vertices + 1);
	m_indices.push_back(vertices + 2);
	m_indices.push_back(vertices + 1);
	m_indices.push_back(vertices + 3);
	m_indices.push_back(vertices + 2);

	// back face
	SetNormal(Vector3::FORWARD * -1.f);
	SetTangent(Vector3::RIGHT * -1.f);
	SetUV( Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex( rightTopBack );
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex( leftTopBack );
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex( rightBottomBack );
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex( leftBottomBack );
	m_indices.push_back(vertices + 4);
	m_indices.push_back(vertices + 5);
	m_indices.push_back(vertices + 6);
	m_indices.push_back(vertices + 5);
	m_indices.push_back(vertices + 7);
	m_indices.push_back(vertices + 6);

	// left face
	SetNormal(Vector3::RIGHT * -1.f);
	SetTangent(Vector3::FORWARD * -1.f);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(leftTopBack);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(leftBottomBack);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(leftBottomFront);
	m_indices.push_back(vertices + 8);
	m_indices.push_back(vertices + 9);
	m_indices.push_back(vertices + 10);
	m_indices.push_back(vertices + 9);
	m_indices.push_back(vertices + 11);
	m_indices.push_back(vertices + 10);

	// right face
	SetNormal(Vector3::RIGHT);
	SetTangent(Vector3::FORWARD);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.maxs.y ));
	PushVertex(rightTopFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( sideUVs.mins.x, sideUVs.mins.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( sideUVs.maxs.x, sideUVs.mins.y ));
	PushVertex(rightBottomBack);	
	m_indices.push_back(vertices + 12);
	m_indices.push_back(vertices + 13);
	m_indices.push_back(vertices + 14);
	m_indices.push_back(vertices + 13);
	m_indices.push_back(vertices + 15);
	m_indices.push_back(vertices + 14);

	// top face
	SetNormal(Vector3::UP);
	SetTangent(Vector3::RIGHT);
	SetUV(Vector2( topUVs.mins.x, topUVs.maxs.y ));
	PushVertex(leftTopBack);
	SetUV(Vector2( topUVs.maxs.x, topUVs.maxs.y ));
	PushVertex(rightTopBack);
	SetUV(Vector2( topUVs.mins.x, topUVs.mins.y ));
	PushVertex(leftTopFront);
	SetUV(Vector2( topUVs.maxs.x, topUVs.mins.y ));
	PushVertex(rightTopFront);

	m_indices.push_back(vertices + 16);
	m_indices.push_back(vertices + 17);
	m_indices.push_back(vertices + 18);
	m_indices.push_back(vertices + 17);
	m_indices.push_back(vertices + 19);
	m_indices.push_back(vertices + 18);

	// bottom face
	SetNormal(Vector3::UP * -1.f);
	SetTangent(Vector3::RIGHT * -1.f);
	SetUV(Vector2( bottomUVs.mins.x, bottomUVs.maxs.y ));
	PushVertex(leftBottomFront);
	SetUV(Vector2( bottomUVs.maxs.x, bottomUVs.maxs.y ));
	PushVertex(rightBottomFront);
	SetUV(Vector2( bottomUVs.mins.x, bottomUVs.mins.y ));
	PushVertex(leftBottomBack);
	SetUV(Vector2( bottomUVs.maxs.x, bottomUVs.mins.y ));
	PushVertex(rightBottomBack);
	m_indices.push_back(vertices + 20);
	m_indices.push_back(vertices + 21);
	m_indices.push_back(vertices + 22);
	m_indices.push_back(vertices + 21);
	m_indices.push_back(vertices + 23);
	m_indices.push_back(vertices + 22);

}


void MeshBuilder::AddSphere(
	  const Vector3& position
	, float radius
	, unsigned int wedges
	, unsigned int slices
	, const Rgba& color /* = Rgba() */ ) {

	SetColor(color);
	unsigned int vertices = (unsigned int) m_vertices.size();

	for (unsigned int slice = 0; slice <= slices; slice++) {
		float v = (float) slice / (float) slices;
		float verticalDegrees = RangeMapFloat(v, 0.f, 1.f, -90.f, 90.f);

		for (unsigned int wedge = 0; wedge <= wedges; wedge++) {
			float u = (float) wedge / (float) wedges;
			float horizontalDegrees = u * 360.f;

			Vector2 uv(u, v);
			Vector3 vertPos = position + PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees);
			SetNormal(PolarToCartesian3D(radius, horizontalDegrees, verticalDegrees));
			SetUV(uv);
			PushVertex(vertPos);
		}
	}

	for (unsigned int wedgeIndex = 0; wedgeIndex < wedges; ++wedgeIndex) {
		for (unsigned int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
			unsigned int bottomLeft = (sliceIndex * (wedges + 1)) + wedgeIndex;
			unsigned int bottomRight = bottomLeft + 1;
			unsigned int topLeft = bottomLeft + wedges + 1;
			unsigned int topRight = topLeft + 1;

			Vector3 bottomRightVertex = m_vertices[bottomRight].position;
			Vector3 bottomLeftVertex = m_vertices[bottomLeft].position;
			Vector3 displacement = bottomRightVertex - bottomLeftVertex;
			Vector3 tangent = displacement.GetNormalized();

			m_vertices[bottomRight].tangent = tangent;
			m_vertices[bottomLeft].tangent = tangent;
			m_vertices[topRight].tangent = tangent;
			m_vertices[topLeft].tangent = tangent;

			m_indices.push_back(vertices + bottomLeft);
			m_indices.push_back(vertices + bottomRight);
			m_indices.push_back(vertices + topRight);
			m_indices.push_back(vertices + bottomLeft);
			m_indices.push_back(vertices + topRight);
			m_indices.push_back(vertices + topLeft);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
VertexMaster& MeshBuilder::GetVertexByIndex( int index ) {
	return m_vertices[index];
}