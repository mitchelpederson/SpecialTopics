#include "Game/GameCommon.hpp"
#include "Game/Map/GameMap.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Entity.hpp"

#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"

GameMap::GameMap( Image const& mapImage ) {
	m_dimensions = mapImage.GetDimensions();

	m_forwardRenderPath = new ForwardRenderPath(g_theRenderer);
	scene = new RenderSceneGraph();

	// Load tiles from the mapImage
	for (int row = 0; row < m_dimensions.y; row++) {
		for (int col = 0; col < m_dimensions.x; col++) {
			m_tiles.push_back( TileDefinition::GetTileByID( mapImage.GetTexel(col, row).r ) );
		}
	}

	player = new Player();


	AddTileMeshesToScene();
	m_minimap = BuildMinimapMesh( mapImage );
	SpawnEntities(mapImage);

	m_minimapCamera = new Camera();
	Texture* tex = new Texture();
	tex->CreateRenderTarget(400, 400);
	tex->SetSamplerMode(SAMPLER_NEAREST);
	m_minimapCamera->SetColorTarget(tex);
	m_minimapCamera->SetProjection(Matrix44::MakeOrtho2D(Vector2(0.f, 0.f), Vector2((float) (m_dimensions.x * 2) * Window::GetInstance()->GetAspectRatio(), (float) (m_dimensions.y * 2))));
	m_minimapCamera->transform.position = Vector3(0.f, 0.f, -15.f);

	m_minimapRenderable = new Renderable();
	m_minimapRenderable->SetMesh(m_minimap);
	m_minimapRenderable->SetMaterial(g_theRenderer->GetMaterial("passthrough"));

	m_playerCamera = new FirstPersonCamera();
	m_playerCamera->SetFrameBuffer(g_theRenderer->GetDefaultFrameBuffer());
	m_playerCamera->SetSkybox(g_theRenderer->CreateCubeMap("Data/Images/galaxy2.png"));
	m_playerCamera->SetProjection(Matrix44::MakeProjection(90.f, Window::GetInstance()->GetAspectRatio(), 0.1f, 100.f));

	m_entities.push_back(player);
	g_theRenderer->CreateOrGetTexture("Data/Images/wolfenstein_textures.png")->SetSamplerMode(SAMPLER_NEAREST);
	g_theRenderer->CreateOrGetTexture("Data/Images/enemysheet.png")->SetSamplerMode(SAMPLER_NEAREST);
	g_theRenderer->CreateOrGetTexture("Data/Fonts/Wolfenstein.png")->SetSamplerMode(SAMPLER_NEAREST);
	g_theRenderer->CreateOrGetTexture("Data/Images/weapons.png")->SetSamplerMode(SAMPLER_NEAREST);

}


void GameMap::SpawnEntities( Image const& mapImage ) {
	for (int row = 0; row < m_dimensions.y; row++) {
		for (int col = 0; col < m_dimensions.x; col++) {

			unsigned char blueChannel = mapImage.GetTexel(col, row).b;
			if (blueChannel != 0 && blueChannel != 100) {
				SpawnEntityFromIDOnTile(blueChannel, IntVector2(col, row));
			}
			if (blueChannel == 100) {
				player->SetPosition(Vector2(col + 0.5f, row + 0.5f));
			}

		}
	}
}


void GameMap::SpawnEntityFromIDOnTile( unsigned char id, IntVector2 const& coord ) {
	Entity* entity = new Entity(id);
	entity->SetPosition(Vector2(coord.x + 0.5f, coord.y + 0.5f));
	m_entities.push_back(entity);
}


void GameMap::AddTileMeshesToScene() {

	for (int col = 0; col < m_dimensions.x; col++) {
		for (int row = 0; row < m_dimensions.y; row++) {
			if (IsTileVisible( IntVector2( col, row ) )) {

				Renderable* tileRenderable = new Renderable();
				Mesh* tileMesh = new Mesh();
				tileRenderable->SetMaterial(g_theRenderer->GetMaterial("tile"));
				tileRenderable->SetMesh(tileMesh);

				MeshBuilder mb;
				mb.Begin(TRIANGLES, false);

				TileDefinition* currentDef = m_tiles[row * m_dimensions.x + col];
				if (currentDef->IsSolid()) {
					AABB2 wallUVs = currentDef->GetWallUVs();

					// front
					// bl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row));

					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row));

					// tr 
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row));
					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row));

					// left
					// bl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row + 1));

					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row + 1));

					// tr 
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row + 1));
					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row));

					// right
					// bl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row + 1));

					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row + 1));

					// tr 
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row + 1));
					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row));

					// back
					//bl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row + 1));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row + 1));

					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row + 1));

					// br
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row + 1));

					// tr 
					mb.SetUV(Vector2(wallUVs.maxs.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row + 1));
					// tl
					mb.SetUV(Vector2(wallUVs.mins.x, wallUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row + 1));

				}
				else {

					AABB2 floorUVs = currentDef->GetFloorUVs();
					AABB2 ceilingUVs = currentDef->GetCeilingUVs();

					// front
					// bl
					mb.SetUV(Vector2(floorUVs.mins.x, floorUVs.mins.y));
					mb.PushVertex(Vector3(col, 0.f, row));

					// br
					mb.SetUV(Vector2(floorUVs.maxs.x, floorUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row));

					// tl
					mb.SetUV(Vector2(floorUVs.mins.x, floorUVs.maxs.y));
					mb.PushVertex(Vector3(col, 0.f, row + 1));

					// br
					mb.SetUV(Vector2(floorUVs.maxs.x, floorUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row));

					// tr 
					mb.SetUV(Vector2(floorUVs.maxs.x, floorUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 0.f, row + 1));

					// tl
					mb.SetUV(Vector2(floorUVs.mins.x, floorUVs.maxs.y));
					mb.PushVertex(Vector3(col, 0.f, row + 1));

					// bl
					mb.SetUV(Vector2(ceilingUVs.mins.x, ceilingUVs.mins.y));
					mb.PushVertex(Vector3(col, 1.f, row));

					// br
					mb.SetUV(Vector2(ceilingUVs.maxs.x, ceilingUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row));

					// tl
					mb.SetUV(Vector2(ceilingUVs.mins.x, ceilingUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row + 1));

					// br
					mb.SetUV(Vector2(ceilingUVs.maxs.x, ceilingUVs.mins.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row));

					// tr 
					mb.SetUV(Vector2(ceilingUVs.maxs.x, ceilingUVs.maxs.y));
					mb.PushVertex(Vector3(col + 1, 1.f, row + 1));

					// tl
					mb.SetUV(Vector2(ceilingUVs.mins.x, ceilingUVs.maxs.y));
					mb.PushVertex(Vector3(col, 1.f, row + 1));
				}

				mb.End();
				tileMesh->FromBuilderAsType<Vertex3D_Lit>(&mb);
				scene->AddRenderable(tileRenderable);
			}
		}
	}
}


Mesh* GameMap::BuildMinimapMesh( Image const& image ) {
	Mesh* minimap = new Mesh();
	MeshBuilder minimapBuilder;
	minimapBuilder.Begin(TRIANGLES, false);

	for (int row = 0; row < m_dimensions.y; row++) {
		for (int col = 0; col < m_dimensions.x; col++) {
			Vector3 bl( (float) col,	 (float) row,	  1.f );
			Vector3 br( (float) col + 1, (float) row,	  1.f );
			Vector3 tl( (float) col,	 (float) row + 1, 1.f );
			Vector3 tr( (float) col + 1, (float) row + 1, 1.f );
			minimapBuilder.SetColor( m_tiles[row * m_dimensions.x + col]->GetMinimapColor() );
			minimapBuilder.PushQuad(bl, br, tr, tl);
		}
	}

	minimapBuilder.End();
	minimap->FromBuilderAsType<Vertex3D_PCU>(&minimapBuilder);

	return minimap;
}



void GameMap::Update() {

	if (g_theInputSystem->WasKeyJustPressed('M')) {
		m_shouldDisplayMinimap = !m_shouldDisplayMinimap;
	}

	if (g_theInputSystem->WasKeyJustPressed('N')) {
		m_isMinimapZoomed = !m_isMinimapZoomed;
	}

	CorrectEntityCollisions();
	
	for (unsigned int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
		m_entities[entityIndex]->Update();
	}

	SortEntitiesByDistanceToPlayer();
	m_playerCamera->transform.position = Vector3(player->m_position.x, 0.5f, player->m_position.y);
	m_playerCamera->transform.euler.y = -player->m_orientationDegrees + 90.f;
	m_playerCamera->Update();

	DeleteDeadEntities();
}


void GameMap::DeleteDeadEntities() {
	for ( int entityIndex = m_entities.size() - 1; entityIndex >= 0; entityIndex-- ) {
		if (m_entities[entityIndex]->m_isDeleteable) {
			delete m_entities[entityIndex];
			m_entities[entityIndex] = m_entities[m_entities.size() - 1];
			m_entities.pop_back();
		}
	}
}


void GameMap::PushEntityOutOfTile( Entity* entity, IntVector2 const& tileCoords ) {

	if (IsTileSolid(tileCoords)) {
		AABB2 tileBounds( tileCoords.x, tileCoords.y, tileCoords.x + 1, tileCoords.y + 1 );
		Vector2 nearestPointOnBounds = tileBounds.GetNearestPointOnBounds(entity->m_position);

		Vector2 displacement = entity->m_position - nearestPointOnBounds;
		float distance = displacement.GetLength();

		if (distance > 0.f && distance < entity->m_physicalRadius) {
			entity->m_position = entity->m_position + (entity->m_physicalRadius - distance) * displacement.GetNormalized();
		}
	}
}


bool GameMap::IsTileSolid( IntVector2 const& tileCoords ) const {
	return m_tiles[tileCoords.y * m_dimensions.x + tileCoords.x]->IsSolid();
}


bool GameMap::IsTileVisible( IntVector2 tileCoords ) const {
	IntVector2 up		= tileCoords + IntVector2( 0,  1);
	IntVector2 down		= tileCoords + IntVector2( 0, -1);
	IntVector2 left		= tileCoords + IntVector2(-1,  0);
	IntVector2 right	= tileCoords + IntVector2( 1,  0);

	if ( up.y != m_dimensions.y && !IsTileSolid(up)) {
		return true;
	}
	if ( down.y != -1 && !IsTileSolid(down)) {
		return true;
	}
	if ( left.x != -1 && !IsTileSolid(left)) {
		return true;
	}
	if ( right.x != m_dimensions.x && !IsTileSolid(right)) {
		return true;
	}
	return false;
}


void GameMap::Render() const {

	m_forwardRenderPath->RenderSceneForCamera(m_playerCamera, scene);

	for (unsigned int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
		m_entities[entityIndex]->Render();
	}

	RenderMinimap();
}


void GameMap::RenderMinimap() const {
	g_theRenderer->SetCamera(m_minimapCamera);
	g_theRenderer->DrawRenderable(m_minimapRenderable);

	for (unsigned int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
		m_entities[entityIndex]->RenderMinimap();
	}

	if (m_shouldDisplayMinimap) {
		float minimapSize = 20.f;
		if (m_isMinimapZoomed) {
			minimapSize = 40.f;
		}
		player->RenderMinimap();
		g_theRenderer->SetShader(nullptr);
		g_theRenderer->EnableDepth(COMPARE_ALWAYS, false);
		g_theRenderer->SetCameraToUI();
		g_theRenderer->DrawTexturedAABB(AABB2(0.f, 0.f, minimapSize, minimapSize * Window::GetInstance()->GetAspectRatio()), *m_minimapCamera->m_frameBuffer->m_colorTarget, Vector2(1.f, 0.f), Vector2(0.f, 1.f), Rgba());
	}
}


void GameMap::CorrectEntityCollisions() {

	for (unsigned int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
		Entity* thisEntity = m_entities[entityIndex];

		if (thisEntity->IsAlive()) {
			for(unsigned int otherEntityIndex = 0; otherEntityIndex < m_entities.size(); otherEntityIndex++) {
				if (entityIndex != otherEntityIndex) {
					Entity* otherEntity = m_entities[otherEntityIndex];

					if (otherEntity->IsAlive()) {
						// check if they are intersecting
						Vector2 displacement = thisEntity->m_position - otherEntity->m_position;
						float distance = displacement.GetLength();
						float radii = thisEntity->m_physicalRadius + otherEntity->m_physicalRadius;
						if (distance < radii) {

							// Apply correction
							float distanceToPushEach = (radii - distance) * 0.5f;
							Vector2 pushDirection = displacement.GetNormalized();
							thisEntity->m_position = thisEntity->m_position + (pushDirection * distanceToPushEach);
							otherEntity->m_position = otherEntity->m_position + (pushDirection * distanceToPushEach * -1.f);

						}
					}
				}
			}
		}
	}
}


Camera* GameMap::GetPlayerCamera() {
	return m_playerCamera;
}


void GameMap::SortEntitiesByDistanceToPlayer() {
	for (unsigned int entityIndex = 0; entityIndex < m_entities.size() - 1; entityIndex++) {
		if (m_entities[entityIndex] != player) {
			float thisDistanceToPlayer = (player->m_position - m_entities[entityIndex]->m_position).GetLengthSquared();
			float nextDistanceToPlayer = (player->m_position - m_entities[entityIndex+1]->m_position).GetLengthSquared();
			if (thisDistanceToPlayer < nextDistanceToPlayer) {
				Entity* temp = m_entities[entityIndex];
				m_entities[entityIndex] = m_entities[entityIndex + 1];
				m_entities[entityIndex + 1] = temp;
			}
		}
	}
}


RaycastResult GameMap::Raycast( Vector2 const& startPosition, float direction ) const {
	Vector2 rayStep = Vector2::MakeDirectionAtDegrees(direction) * 0.1f;

	Vector2 currentStep = startPosition + rayStep;
	RaycastResult result;

	while (!result.hitWall && !result.hitEntity) {
		// Check wall
		result.hitWall = IsTileSolid( IntVector2(currentStep) );
		if (result.hitWall) {
			result.location = currentStep;
		}

		// Check entities
		result.entity = GetEntityAtPoint( currentStep );
		if (result.entity != nullptr && result.entity->IsAlive()) {
			result.hitEntity = true;
			result.location = currentStep;
		}
		else {
			result.entity = nullptr;
		}

		currentStep = currentStep + rayStep;
	}

	return result;
}


Entity* GameMap::GetEntityAtPoint( Vector2 const& point ) const {

	for (unsigned int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++) {
		Entity* current = m_entities[entityIndex];
		if ((current->GetPosition() - point).GetLengthSquared() < current->m_physicalRadius * current->m_physicalRadius) {
			return current;
		}
	}

	return nullptr;
}