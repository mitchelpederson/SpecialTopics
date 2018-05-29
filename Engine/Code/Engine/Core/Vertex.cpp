#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"

const VertexAttribute Vertex3D_PCU::s_attributes[] = {
	VertexAttribute( "POSITION", RENDER_DATA_FLOAT, 3, false, offsetof(Vertex3D_PCU, position), sizeof(Vertex3D_PCU)),
	VertexAttribute( "COLOR", RENDER_DATA_UNSIGNED_BYTE, 4, true, offsetof(Vertex3D_PCU, color), sizeof(Vertex3D_PCU)),
	VertexAttribute( "UV", RENDER_DATA_FLOAT, 2, false, offsetof(Vertex3D_PCU, uv), sizeof(Vertex3D_PCU))
};

const VertexAttribute Vertex3D_Lit::s_attributes[] {
	VertexAttribute( "POSITION", RENDER_DATA_FLOAT, 3, false, offsetof(Vertex3D_Lit, position), sizeof(Vertex3D_Lit)),
	VertexAttribute( "COLOR", RENDER_DATA_UNSIGNED_BYTE, 4, true, offsetof(Vertex3D_Lit, color), sizeof(Vertex3D_Lit)),
	VertexAttribute( "UV", RENDER_DATA_FLOAT, 2, false, offsetof(Vertex3D_Lit, uv), sizeof(Vertex3D_Lit)),
	VertexAttribute( "NORMAL", RENDER_DATA_FLOAT, 3, false, offsetof(Vertex3D_Lit, normal), sizeof(Vertex3D_Lit)),
	VertexAttribute( "TANGENT", RENDER_DATA_FLOAT, 3, false, offsetof(Vertex3D_Lit, tangent), sizeof(Vertex3D_Lit))

};

const VertexLayout Vertex3D_PCU::LAYOUT = VertexLayout(sizeof(Vertex3D_PCU), Vertex3D_PCU::s_attributes, 3);
const VertexLayout Vertex3D_Lit::LAYOUT = VertexLayout(sizeof(Vertex3D_Lit), Vertex3D_Lit::s_attributes, 5);


VertexLayout::VertexLayout( unsigned int vertStride, const VertexAttribute attributes[], int numAttributes ) {

	for (int i = 0; i < numAttributes; i++) {
		m_attributes.push_back(attributes[i]);
	}

	stride = vertStride;
}


unsigned int VertexLayout::GetAttributeCount() const {
	return m_attributes.size();
}

const VertexAttribute& VertexLayout::GetAttribute(unsigned int index) const {
	return m_attributes[index];
}