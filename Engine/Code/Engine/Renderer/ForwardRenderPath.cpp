#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/DebugRender.hpp"

struct LightComparisonData {
	unsigned int index;
	float weight;
};


void ForwardRenderPath::Render( RenderSceneGraph* scene ) {
	scene->SortCameras();

	for ( Camera* cam : scene->m_cameras ) {
		RenderSceneForCamera( cam, scene );
	}
}


void ForwardRenderPath::RenderSceneForCamera( Camera* camera, RenderSceneGraph* scene ) {
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


void ForwardRenderPath::EnableLightsForDrawCall( const DrawCall& drawCall, RenderSceneGraph* scene ) {
	int maxLights = min(drawCall.m_lightCount, scene->m_lights.size());
	for (int i = 0; i < maxLights; i++) {
		int lightIndexToUse = drawCall.m_lightIndices[i];
		renderer->SetLight(i, *scene->m_lights[lightIndexToUse]);
	}
}


void ForwardRenderPath::ClearBasedOnCameraOptions( Camera* camera ){
	renderer->ClearScreen(Rgba(0, 0, 0, 255));
	renderer->ClearDepth();
}


void ForwardRenderPath::ComputeMostContributingLights( unsigned int* m_lightCount, unsigned int m_lightIndices[MAX_LIGHTS], const Vector3& position, RenderSceneGraph* scene ) {
	
	/*for (int i = 0; i < scene->GetLightCount(); i++) {
		DebugRenderWireSphere(0.f, scene->m_lights[i]->m_position, 0.2f, scene->m_lights[i]->m_color );
	}*/

	unsigned int numLights = scene->m_lights.size();
	LightComparisonData* lights = new LightComparisonData[numLights];

	// Get the distance to each light
	for (int sceneLightIndex = 0; sceneLightIndex < numLights; sceneLightIndex++) {
		const Light& light = *scene->m_lights[sceneLightIndex];
		lights[sceneLightIndex].index = sceneLightIndex;
		lights[sceneLightIndex].weight = light.m_intensity / ( 1.f + (light.m_attenuation * light.m_position.DistanceFrom(position)) );
	}
	
	// Sort by distance, i'm extremely lazy and am using a bubble sort for now. will improve when slow
	for (int i = 0; i < numLights; i++) {
		for (int j = 0; j < numLights - i - 1; j++) {
			if (lights[j].weight < lights[j+1].weight) {
				LightComparisonData temp = lights[j];
				lights[j] = lights[j+1];
				lights[j+1] = temp;
			}
		}
	}

	// Copy the closest 8 or all lights if there is less than 8
	for (int i = 0; i < numLights; i++) {
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

	// Sort based on the queue, so we can draw opaque before transparent things
	for (int i = 0; i < drawCalls.size(); i++) {
		for (int j = 0; j < drawCalls.size() - 1; j++) {
			if (drawCalls[j].m_queue > drawCalls[j+1].m_queue) {
				DrawCall temp = drawCalls[j];
				drawCalls[j] = drawCalls[j+1];
				drawCalls[j+1] = temp;
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

	if (m_effectCurrentTarget == nullptr) {
		m_effectCurrentTarget = camera->m_frameBuffer->m_colorTarget;
	}

	m_effectScratchTarget = Texture::CreateDuplicateTarget( m_effectCurrentTarget );
	m_effectCamera->SetColorTarget(  );
}