#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include <string>
#include <vector>

enum RenderDataType;

struct VertexMaster {
	Vector3 position;
	Rgba color;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
};

class VertexAttribute {
public:
	VertexAttribute( const std::string& name, RenderDataType type, unsigned int count, bool normalized, unsigned int offset, unsigned int vertStride )
		: handle(name)
		, dataType(type)
		, elementCount(count)
		, isNormalized(normalized)
		, memberOffset(offset)
		, vertexStride(vertStride) 
	{}


public:
	std::string handle; // Position
	int memberOffset; // how far from start of element to this piece of data
	RenderDataType dataType; // The data type (GLint, GLfloat, etc.)
	bool isNormalized;
	int elementCount;
	int vertexStride;
};


class VertexLayout {
public:
	VertexLayout( unsigned int vertStride, const VertexAttribute attributes[], int numAttributes );
	unsigned int GetAttributeCount() const;
	const VertexAttribute& GetAttribute(unsigned int index) const;

	std::vector<VertexAttribute> m_attributes;
	unsigned int stride; // how far between elements

};


struct Vertex3D_PCU {
public:
	Vector3 position;
	Rgba	color;
	Vector2 uv;

	Vertex3D_PCU(Vector3 pos, Vector2 uvs, Rgba color) 
		: position(pos)
		, uv(uvs)
		, color(color) 
	{}
	Vertex3D_PCU( const VertexMaster& master ) {
		position = master.position;
		color = master.color;
		uv = master.uv;
	}

	Vertex3D_PCU() : position(Vector3()), uv(Vector2()), color(Rgba()) {}
	static const VertexAttribute s_attributes[];
	static const int NUM_ATTRIBUTES = 3;
	static const VertexLayout LAYOUT;
};


struct Vertex3D_Lit {
	Vector3 position;
	Rgba	color;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;

	Vertex3D_Lit( const VertexMaster& master ) {
		position = master.position;
		color = master.color;
		uv = master.uv;
		normal = master.normal;
		tangent = master.tangent;
	}

	Vertex3D_Lit( const Vector3& pos, const Rgba& col, const Vector2& u, const Vector3& norm, const Vector3& tang ) {
		position = pos;
		color = col;
		uv = u;
		normal = norm;
		tangent = tang;
	}
	Vertex3D_Lit() {
		position = Vector3();
		color = Rgba();
		uv = Vector2();
		normal = Vector3();
		tangent = Vector3();
	}

	static const VertexAttribute s_attributes[];
	static const int NUM_ATTRIBUTES = 5;
	static const VertexLayout LAYOUT;
};
/*


VertexAttribute Vertex3D_PCU::s_layout[] = {
	VertexAttribute( "POSITION", GL_FLOAT, 3, false, offsetof(Vertex3D_PCU, position)),
	VertexAttribute( "COLOR", GL_UNSIGNED_BYTE, 4, false, offsetof(Vertex3D_PCU, color)),
	VertexAttribute( "uv", GL_FLOAT, 2, false, offsetof(Vertex3D_PCU, uv)),
	VertexAttribute( )
};*/




