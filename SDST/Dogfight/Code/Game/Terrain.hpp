#pragma once

#include "Engine/Renderer/Camera.hpp"


class Terrain {

public:
	Terrain( Camera* cam );
	~Terrain();

	void Update();
	void SetMesh( Mesh* mesh );

	bool IsPointBelowTerrain( const Vector3& pos );


private:
	void RebuildTerrainMesh();
	void CreateRebuildJob();


public:
	int verticesPerSide = 500;
	float meshLength = 200000.f;
	float maxHeight = 3000.f;

	
private:

	Camera* m_camera = nullptr;

	Renderable* m_terrainRenderable = nullptr;
	Mesh* m_terrainMesh = nullptr;
	Vector3 m_positionLastRebuild;

	int m_terrainRebuildJobID = -1;
};