#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <vector>
#include <string.h>
#include <fstream>


void MeshBuilder::Begin(DrawPrimitive primitive, bool useIndices) {
	m_instructions.type = primitive;
	m_instructions.useIndices = useIndices;

	if (useIndices) {
		m_instructions.startIndex = m_indices.size();
	}
	else {
		m_instructions.startIndex = m_vertices.size();
	}
}


void MeshBuilder::End() {
	unsigned int endIndex;
	if (m_instructions.useIndices) {
		endIndex = m_indices.size();
		m_instructions.indexCount = endIndex - m_instructions.startIndex;
	} else {
		endIndex = m_vertices.size();
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
	return m_vertices.size() - 1;
}


unsigned int MeshBuilder::PushQuad( const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl) {
	unsigned int firstIndex = m_vertices.size();
	
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



unsigned int MeshBuilder::GetVertexCount() const {
	return m_vertices.size() - m_instructions.startIndex;
}

unsigned int MeshBuilder::GetIndexCount() const {
	return m_indices.size();
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

		if (tokens[0] == "v") {
			vertPositions.push_back( Vector3( std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]) ) );
		}

		else if (tokens[0] == "vn") {
			vertNormals.push_back( Vector3( std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) ) );
		} 

		else if (tokens[0] == "vt") {
			vertUVs.push_back( Vector2( std::stof(tokens[1]), std::stof(tokens[2]) ) );
		}

		else if (tokens[0] == "f") {

			std::vector<unsigned int> faceIndices;
			for (unsigned int i = 1; i < tokens.size() - 1; i++) {
				std::vector<std::string> faceTokens = SplitString(tokens[i], '/');
				int posIndex = std::stoi(faceTokens[0]) - 1;
				int normalIndex = std::stoi(faceTokens[2]) - 1;
				int uvIndex = std::stoi(faceTokens[1]) - 1;

				SetNormal(vertNormals[normalIndex]);
				SetUV(vertUVs[uvIndex]);
				unsigned int index = PushVertex(vertPositions[posIndex]);
				faceIndices.push_back(index);
			}

				// Triangle
			if (faceIndices.size() == 3) {
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

	// front face
	SetNormal(Vector3::FORWARD * -1.f);
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
	SetNormal(Vector3::FORWARD);
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

	Vector3 right = basis.GetRight();
	Vector3 up = basis.GetUp();
	Vector3 forward = basis.GetForward();

	vertices[0].position = position;
	vertices[0].uv = Vector2();
	vertices[0].color = Rgba(255, 0, 0, 255);
	vertices[1].position = right + position;
	vertices[1].uv = Vector2();
	vertices[1].color = Rgba(255, 0, 0, 255);

	vertices[2].position = position;
	vertices[2].uv = Vector2();
	vertices[2].color = Rgba(0, 255, 0, 255);
	vertices[3].position = up + position;
	vertices[3].uv = Vector2();
	vertices[3].color = Rgba(0, 255, 0, 255);

	vertices[4].position = position;
	vertices[4].uv = Vector2();
	vertices[4].color = Rgba(0, 120, 255, 255);
	vertices[5].position = forward + position;
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

	int seed = GetRandomIntInRange(0, 1000);

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