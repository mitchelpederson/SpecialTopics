#pragma once
#include "Game/Terrain.hpp"

#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Async/Job.hpp"


class TerrainRebuildJob : public Job {
public:
	TerrainRebuildJob( const Vector3& playerPosition, Terrain* terrain );
	~TerrainRebuildJob();

	virtual void Execute() override;
	virtual void OnComplete() override;


private:
	void GenerateMeshWorking();
	void GenerateMeshBetterTangents();

	Vector3 m_centerVertexPosition;
	Terrain* m_terrain;
	MeshBuilder m_mb;
};