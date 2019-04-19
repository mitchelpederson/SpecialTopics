#include "Game/Terrain.hpp"
#include "Game/Jobs/TerrainRebuildJob.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/SmoothNoise.hpp"


//----------------------------------------------------------------------------------------------------------------
Terrain::Terrain( Camera* cam ) 
	: m_camera( cam )
{
	CreateRebuildJob();
	g_theRenderer->CreateOrGetTexture("Data/Images/grass01.png")->SetSamplerMode(SAMPLER_LINEAR_MIPMAP_LINEAR);
}


//----------------------------------------------------------------------------------------------------------------
Terrain::~Terrain() {
	if ( m_terrainMesh != nullptr ) {
		delete m_terrainMesh;
		m_terrainMesh = nullptr;
		m_terrainRenderable->SetMesh( nullptr );
	}

	if ( m_terrainRenderable != nullptr ) {
		delete m_terrainRenderable;
		m_terrainRenderable = nullptr;
	}

	m_camera = nullptr;
}


//----------------------------------------------------------------------------------------------------------------
void Terrain::Update() {

	if ( m_terrainRebuildJobID != -1 ) {
		Job* claimedBuildJob = g_theJobSystem->ClaimFinishedJob( m_terrainRebuildJobID ); 
		if ( claimedBuildJob != nullptr ) {
			m_terrainRebuildJobID = -1;
			delete claimedBuildJob;
			claimedBuildJob = nullptr;
		}
	}

	if ( (m_camera->transform.position - m_positionLastRebuild).GetLength() > 5000.f ) {
		CreateRebuildJob();
	}
}


//----------------------------------------------------------------------------------------------------------------
void Terrain::SetMesh( Mesh* mesh ) {

	if ( m_terrainRenderable == nullptr ) {
		m_terrainRenderable = new Renderable();
		m_terrainRenderable->SetMaterial( g_theRenderer->GetMaterial("terrain") );
		g_theGame->GetMultiplayerState()->m_scene->AddRenderable(m_terrainRenderable);
	}

	if ( m_terrainMesh != nullptr ) {
		delete m_terrainMesh;
		m_terrainMesh = nullptr;
	}


	m_terrainMesh = mesh;
	m_terrainRenderable->SetMesh( m_terrainMesh );
}


//----------------------------------------------------------------------------------------------------------------
void Terrain::CreateRebuildJob() {
	TerrainRebuildJob* rebuildJob = new TerrainRebuildJob( m_camera->transform.position, this );
	m_terrainRebuildJobID = g_theJobSystem->SubmitJob( rebuildJob );
	m_positionLastRebuild = m_camera->transform.position;
}


//----------------------------------------------------------------------------------------------------------------
bool Terrain::IsPointBelowTerrain( const Vector3& pos ) {
	float height = RangeMapFloat( SmoothStart2( Compute2dPerlinNoise( pos.x, pos.z, maxHeight, 3 ) ), -1.f, 1.f, 0.f, maxHeight );
	return pos.y < height;
}