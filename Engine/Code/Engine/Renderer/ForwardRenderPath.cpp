#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/ProfilerScopedLog.hpp"
#include "Engine/Profiler/Profiler.hpp"

struct LightComparisonData {
	unsigned int index;
	float weight;
};


ForwardRenderPath::ForwardRenderPath( Renderer* r ) : renderer( r ) {
	m_effectCamera = new Camera();
	m_effectCamera->SetProjectionOrtho(1.f, -1.f, 1.f);
}


void ForwardRenderPath::Render( RenderSceneGraph* scene ) {
	PROFILER_SCOPED_PUSH();
	scene->SortCameras();

	for ( Camera* cam : scene->m_cameras ) {
		RenderSceneForCamera( cam, scene );
	}
}


void ForwardRenderPath::RenderSceneForCamera( Camera* camera, RenderSceneGraph* scene ) {

	PROFILER_SCOPED_PUSH();

	for ( Light* light : scene->m_lights ) {
		if (light->m_isShadowcasting > 0.f) {
			RenderShadowCastingObjectsForLight( light, scene, camera );
		}
	}
	renderer->SetCamera( camera );
	ClearBasedOnCameraOptions( camera );

	if (camera->skybox != nullptr) {
		renderer->DrawRenderable(camera->skybox);
	}

	for ( ParticleEmitter* particleEmitter : scene->m_particleEmitters ) {
		particleEmitter->PreRender( camera );
	}

	std::vector<DrawCall> drawCalls;
	for( Renderable* renderable : scene->m_renderables ) {
		DrawCall dc;
		ComputeMostContributingLights( &(dc.m_lightCount), dc.m_lightIndices, renderable->GetPosition(), scene );
		dc.m_model = renderable->GetModelMatrix();
		dc.m_mesh = renderable->GetMesh();
		dc.m_material = renderable->GetMaterial();
		dc.m_layer = 0;
		dc.m_queue = dc.m_material->GetQueue();
		drawCalls.push_back(dc);
	}

	SortDrawCalls( drawCalls, camera );

	for( DrawCall& drawCall : drawCalls ) {
		EnableLightsForDrawCall( drawCall, scene );
		renderer->Draw( drawCall );
	}

	ApplyCameraEffects(camera);

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F11)) {
		g_theRenderer->SaveScreenshot();
	}
}


void ForwardRenderPath::RenderShadowCastingObjectsForLight( Light* light, RenderSceneGraph* scene, Camera* currentCamera ) {
	PROFILER_SCOPED_PUSH();
	Camera cam;

	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("depth-only"));


	Vector3 playerCamPos = currentCamera->transform.position;
	Vector3 playerCamForward = currentCamera->m_cameraMatrix.GetForward();
	Vector3 shadowCamPos = playerCamPos + (playerCamForward * 10.f);

	cam.SetProjection(Matrix44::MakeOrthographic(32.f, -32.f, // left, right
												 32.f, -32.f, // top, bottom
												 50.f, -50.f)); // far, near

	cam.transform.position = playerCamPos;
	cam.transform.LookToward(light->m_direction.GetNormalized() * -1.f, Vector3::UP);
	cam.m_viewMatrix = cam.transform.GetWorldToLocalMatrix();
	cam.m_cameraMatrix = cam.transform.GetLocalToWorldMatrix();
	
	cam.SetColorTarget(nullptr);
	cam.SetDepthStencilTarget(light->CreateOrGetShadowTexture());
	g_theRenderer->SetCamera(&cam);
	Matrix44 camVP = cam.m_projMatrix;
	camVP.Append(cam.m_viewMatrix);
	light->m_viewProjection = camVP;
	light->m_inverseViewProjection = camVP.GetInverse();

	g_theRenderer->SetViewport(0, 0, light->m_shadowMapResolution.x, light->m_shadowMapResolution.y);
	g_theRenderer->ClearDepth();
	for (Renderable* r : scene->m_renderables) {
		g_theRenderer->SetModelMatrix(r->GetModelMatrix());
		g_theRenderer->DrawMesh(r->GetMesh());
	}
	g_theRenderer->SetViewport(0, 0, Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight());

}


void ForwardRenderPath::EnableLightsForDrawCall( const DrawCall& drawCall, RenderSceneGraph* scene ) {
	PROFILER_SCOPED_PUSH();
	int maxLights = (int) min(drawCall.m_lightCount, scene->m_lights.size());
	for (int i = 0; i < maxLights; i++) {
		int lightIndexToUse = drawCall.m_lightIndices[i];
		renderer->SetLight(i, *scene->m_lights[lightIndexToUse]);
	}
}


void ForwardRenderPath::ClearBasedOnCameraOptions( Camera* camera ){
	PROFILER_SCOPED_PUSH();
	renderer->ClearScreen(Rgba(0, 0, 0, 255));
	renderer->ClearDepth();
}


void ForwardRenderPath::ComputeMostContributingLights( unsigned int* m_lightCount, unsigned int m_lightIndices[MAX_LIGHTS], const Vector3& position, RenderSceneGraph* scene ) {
	PROFILER_SCOPED_PUSH();
	unsigned int numLights = (unsigned int) scene->m_lights.size();
	LightComparisonData* lights = new LightComparisonData[numLights];

	// Get the distance to each light
	for (unsigned int sceneLightIndex = 0; sceneLightIndex < numLights; sceneLightIndex++) {
		const Light& light = *scene->m_lights[sceneLightIndex];
		lights[sceneLightIndex].index = sceneLightIndex;
		if (light.m_isPointLight != 0.f) {
			lights[sceneLightIndex].weight = light.m_intensity / ( 1.f + (light.m_attenuation * light.m_position.DistanceFrom(position)) );
		}
		else  {
			lights[sceneLightIndex].weight = light.m_intensity;
		}
	}
	
	// Sort by distance, i'm extremely lazy and am using a bubble sort for now. will improve when slow
	for (unsigned int i = 0; i < numLights; i++) {
		for (unsigned int j = 0; j < numLights - i - 1; j++) {
			if (lights[j].weight < lights[j+1].weight) {
				LightComparisonData temp = lights[j];
				lights[j] = lights[j+1];
				lights[j+1] = temp;
			}
		}
	}

	// Copy the closest 8 or all lights if there is less than 8
	for (unsigned int i = 0; i < numLights; i++) {
		m_lightIndices[i] = lights[i].index;
		*m_lightCount = i + 1;
		if (i == MAX_LIGHTS - 1) {
			delete lights;
			return;
		}
	}	

	delete[] lights;
}


void ForwardRenderPath::SortDrawCalls( std::vector<DrawCall>& drawCalls, Camera* camera ) {
	PROFILER_SCOPED_PUSH();
	// Sort based on the queue, so we can draw opaque before transparent things
	for (int i = 0; i < drawCalls.size(); i++) {
		for (int j = 0; j < drawCalls.size() - 1; j++) {
			bool wasSwapped = false;
			if (drawCalls[j].m_queue > drawCalls[j+1].m_queue) {
				DrawCall temp = drawCalls[j];
				drawCalls[j] = drawCalls[j+1];
				drawCalls[j+1] = temp;
				wasSwapped = true;
			}
			if (wasSwapped == false) {
				break;
			}
		}
	}

	// Find the start of the alpha draw calls
	int alphaStartIndex = -1;
	for (int searchIndex = 0; searchIndex < drawCalls.size(); searchIndex++) {
		if (drawCalls[searchIndex].m_queue == 1) {
			alphaStartIndex = searchIndex;
			break;
		}
	}

	// If there is an alpha draw call, sort by distance to camera
	if (alphaStartIndex != -1) {
		for (int i = alphaStartIndex; i < drawCalls.size(); i++) {
			for (int j = alphaStartIndex; j < drawCalls.size() - 1; j++) {
				DrawCall& current = drawCalls[j];
				DrawCall& next = drawCalls[j+1];
				float distanceToCameraCurrent = (camera->m_cameraMatrix.GetTranslation() - current.m_model.GetTranslation()).GetLengthSquared();
				float distanceToCameraNext    = (camera->m_cameraMatrix.GetTranslation() - next.m_model.GetTranslation()).GetLengthSquared();

				if (distanceToCameraCurrent > distanceToCameraNext) {
					DrawCall temp = drawCalls[j];
					drawCalls[j] = drawCalls[j+1];
					drawCalls[j+1] = temp;
				}
			}
		}
	}
}


void ForwardRenderPath::ApplyCameraEffects( Camera* camera ) {
	PROFILER_SCOPED_PUSH();
	if (camera->cameraEffects.size() == 0) {
		return;
	}
	 
	Texture* defaultSourceTarget = Texture::CreateDuplicateTarget( camera->m_frameBuffer->m_colorTarget );
	Texture* effectScratchTarget = Texture::CreateDuplicateTarget( defaultSourceTarget );
	Texture* effectCurrentSourceTarget = effectScratchTarget; // Start these swapped since the first
	Texture* effectCurrentDestTarget = defaultSourceTarget;   // thing we do in the loop is swap

	m_effectCamera->SetColorTarget( effectCurrentDestTarget );
	renderer->SetCamera(m_effectCamera);
	renderer->CopyFrameBuffer( m_effectCamera->m_frameBuffer, camera->m_frameBuffer );
	renderer->UseTexture(7, *camera->m_frameBuffer->m_depthStencilTarget);

	float halfWidth = Window::GetInstance()->GetWidth() * 0.5f;
	float halfHeight = Window::GetInstance()->GetHeight() * 0.5f;

	for (int effectIndex = 0; effectIndex < camera->cameraEffects.size(); effectIndex++) {
		std::swap(effectCurrentSourceTarget, effectCurrentDestTarget);
		m_effectCamera->SetColorTarget( effectCurrentDestTarget );
		renderer->SetCamera(m_effectCamera);		
		renderer->BindMaterial(camera->cameraEffects[effectIndex]);
		renderer->DrawTexturedAABB(AABB2(halfWidth, -halfHeight, -halfWidth, halfHeight), *effectCurrentSourceTarget, Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba());
		std::swap(effectCurrentSourceTarget, effectCurrentDestTarget);
	}


	// We need to copy the effect camera's contents to the game camera
	renderer->CopyFrameBuffer( camera->m_frameBuffer, m_effectCamera->m_frameBuffer );
	renderer->SetCamera(camera);
	// At the end, clean up
	delete effectScratchTarget;
	effectScratchTarget = nullptr;
	delete defaultSourceTarget;
	defaultSourceTarget = nullptr;
}