#include "Engine/Profiler/ProfilerWindow.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Math/MathUtils.hpp"

ProfilerWindow* ProfilerWindow::instance = nullptr;
bool ProfilerWindow::isOpen = false;
ProfilerWindow::eViewMode ProfilerWindow::currentProfilerViewMode = PROFILER_VIEW_TREE;

void OpenProfiler( const std::string& command ) {
	ProfilerWindow::Open();
}

void CloseProfiler( const std::string& command ) {
	ProfilerWindow::Close();
}

void ProfilerWindow::Initialize() {
	instance = new ProfilerWindow();

	CommandRegistration::RegisterCommand("pf_open", OpenProfiler);
	CommandRegistration::RegisterCommand("pf_close", CloseProfiler);
}


void ProfilerWindow::Update() {
	if (isOpen && !Profiler::IsPaused()) {

		if (g_theInputSystem->WasKeyJustPressed('V')) {
			if (currentProfilerViewMode == PROFILER_VIEW_TREE) {
				currentProfilerViewMode = PROFILER_VIEW_FLAT;
			}
			else {
				currentProfilerViewMode = PROFILER_VIEW_TREE;
			}
		}

		ProfilerMeasurement* previousFrameMeasurement = Profiler::GetPreviousFrame();
		ProfilerReport* frameReport = new ProfilerReport();
		if ( currentProfilerViewMode == PROFILER_VIEW_TREE ) {
			frameReport->GenerateTreeReportFromFrame(previousFrameMeasurement);
		}
		else if ( currentProfilerViewMode == PROFILER_VIEW_FLAT ) {
			frameReport->GenerateFlatReportFromFrame(previousFrameMeasurement);
		}

		savedFrames.push_back(frameReport);
		if (frameReport->GetTotalFrameTime() > maxFrameTimeInHistory) {
			maxFrameTimeInHistory = (float) frameReport->GetTotalFrameTime();
			lastFrameThatSetMax = frameCount;
		}

		if (savedFrames.size() >= PROFILER_MAX_FRAME_HISTORY) {
			delete savedFrames.front();
			savedFrames.pop_front();
		}

		if (frameCount - lastFrameThatSetMax > PROFILER_MAX_FRAME_HISTORY) {
			maxFrameTimeInHistory -= maxFrameTimeInHistory * 0.05f;
			for (unsigned int frameIndex = 0; frameIndex < savedFrames.size(); frameIndex++) {
				if (savedFrames[frameIndex]->GetTotalFrameTime() > maxFrameTimeInHistory) {
					maxFrameTimeInHistory = (float) savedFrames[frameIndex]->GetTotalFrameTime();
					lastFrameThatSetMax = frameCount - ((unsigned int) savedFrames.size() - frameIndex);
				}
			}
		}

		frameCount++;
	}
}


void ProfilerWindow::Render() const {
	if (isOpen) {
		ProfilerReport* latestFrame = savedFrames.back();

		g_theRenderer->SetShader(nullptr);
		g_theRenderer->DisableDepth();
		g_theRenderer->SetCameraToUI();
		RenderBackground();
		RenderGeneralFrameInfo(latestFrame->root);
		RenderReportEntries(latestFrame->root);
		RenderHistoryGraph();
	}
}


void ProfilerWindow::RenderGeneralFrameInfo( ProfilerReportEntry* root ) const {

	g_theRenderer->DrawTextInBox2D(generalRenderBox, Vector2(0.f, 1.f), "FPS: " + std::to_string( 1.0 / root->totalTime), 4.f, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	g_theRenderer->DrawTextInBox2D(generalRenderBox, Vector2(0.f, 0.8f), "Frame Time: " + std::to_string( root->totalTime), 4.f, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	
	std::string reportHeader = Stringf("%-75s %10s %10s %10s %10s", "Function scope and name:", "time Inc", "\% Inc", "time Excl", "\% Excl");
	g_theRenderer->DrawTextInBox2D(generalRenderBox, Vector2(0.f, 0.0f), reportHeader, 2.f, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);

}


void ProfilerWindow::RenderReportEntries( ProfilerReportEntry* root ) const {

	RenderEntry(root, 0, 0);

}

unsigned int ProfilerWindow::RenderEntry( ProfilerReportEntry* node, unsigned int index, unsigned int indent ) const {
	
	// Get the bounds of this line's text box
	float offsetFromTop = entryRenderBox.maxs.y - ((float) (index + 1) * textHeight);
	AABB2 textBox(entryRenderBox.mins.x, offsetFromTop, entryRenderBox.maxs.x, offsetFromTop + textHeight);

	// Format the information in a string
	std::string nodeString = Stringf("%-*s %10f (%8f) %10f (%8f)", 75 - indent, node->id.c_str(), node->totalTime, node->percentTime, node->selfTime, node->selfPercentTime);
	std::string indentedNodeString = "";
	for (unsigned int i = 0; i < indent; i++) {
		indentedNodeString += " ";
	}
	indentedNodeString += nodeString;

	g_theRenderer->DrawTextInBox2D( textBox, Vector2(0.f, 0.f), indentedNodeString, textHeight, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN ); 
	
	// Render the children of this node
	std::map<std::string, ProfilerReportEntry*>::iterator it = node->children.begin();
	while (it != node->children.end()) {
		index = RenderEntry(it->second, index + 1, indent + 1);
		it++;
	}
	return index;
}


void ProfilerWindow::RenderBackground() const {
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 60, 100, 180));
}


void ProfilerWindow::RenderHistoryGraph() const {

	g_theRenderer->DrawAABB(historyRenderBox, Rgba(50, 50, 50, 255));

	float sliceWidth =  (historyRenderBox.maxs.x - historyRenderBox.mins.x) / (float) (savedFrames.size() - 1);

	// Create a mesh for the graph
	MeshBuilder mb;
	mb.Begin(TRIANGLES, false);
	mb.SetColor(Rgba(200, 100, 50, 255));
	for (unsigned int frameIndex = 0; frameIndex < savedFrames.size() - 1; frameIndex++) {
		
		ProfilerReport* currentFrameReport = savedFrames[frameIndex];
		ProfilerReport* nextFrameReport = savedFrames[frameIndex + 1];

		float currentOffsetOfLeft = sliceWidth * (float) frameIndex + historyRenderBox.mins.x;
		float currentReportVertexHeight = RangeMapFloat((float) currentFrameReport->GetTotalFrameTime(), 0.f, maxFrameTimeInHistory, historyRenderBox.mins.y, historyRenderBox.maxs.y);
		float nextReportVertexHeight = RangeMapFloat((float) nextFrameReport->GetTotalFrameTime(), 0.f, maxFrameTimeInHistory, historyRenderBox.mins.y, historyRenderBox.maxs.y);
	
		Vector3 bl( currentOffsetOfLeft, historyRenderBox.mins.y, 0.f );
		Vector3 br( currentOffsetOfLeft + sliceWidth, historyRenderBox.mins.y, 0.f );
		Vector3 tl( currentOffsetOfLeft, currentReportVertexHeight, 0.f);
		Vector3 tr( currentOffsetOfLeft + sliceWidth, nextReportVertexHeight, 0.f);

		mb.PushQuad(bl, br, tr, tl);
		
	}
	mb.End();

	// Send the mesh builder data to the mesh and draw it
	Mesh* graphMesh = new Mesh();
	graphMesh->FromBuilderAsType<Vertex3D_PCU>(&mb);
	g_theRenderer->DrawMesh(graphMesh);
	delete graphMesh;
	graphMesh = nullptr;

}


ProfilerWindow* ProfilerWindow::GetInstance() {
	return instance;
}

void ProfilerWindow::Open() {
	isOpen = true;
}

void ProfilerWindow::Close() {
	isOpen = false;
}