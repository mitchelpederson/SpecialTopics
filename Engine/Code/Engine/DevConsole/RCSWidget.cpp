#include "Engine/DevConsole/RCSWidget.hpp"
#include "Engine/DevConsole/RemoteCommandService.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommon.hpp"


#include <string>


//----------------------------------------------------------------------------------------------------------------
RCSWidget::RCSWidget() {

}


//----------------------------------------------------------------------------------------------------------------
RCSWidget::~RCSWidget() {

}


//----------------------------------------------------------------------------------------------------------------
void RCSWidget::Update() {

}


//----------------------------------------------------------------------------------------------------------------
void RCSWidget::Render() const {
	std::string header = "RCS System ";
	
	if ( RemoteCommandService::IsConnected() ) {
		header += "[CONNECTED]";
	} else if ( RemoteCommandService::IsHost() ) {
		header += "[HOSTING]";
	} else {
		header += "[READY]";
	}

	// Print our local IP and our current state
	std::string myIP = "Local IP: " + NetAddress_T::GetLocal(29283).to_string();
	g_theRenderer->DrawTextInBox2D( AABB2( 75.f, 95.f, 100.f, 100.f), Vector2(0.f, 0.5f), header.c_str(), 2.f, Rgba(), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );	
	g_theRenderer->DrawTextInBox2D( AABB2( 75.f, 92.f, 100.f, 95.f), Vector2(0.f, 0.5f), myIP, 1.5f, Rgba(), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );

	// If hosting, show all the clients connected to us
	if ( RemoteCommandService::IsHost() ) {

		std::vector<NetAddress_T> clientAddresses = RemoteCommandService::GetClientAddresses();
		for ( unsigned int index = 0; index < clientAddresses.size(); index++ ) {
			std::string client = "[" + std::to_string(index) + "] ";
			client += clientAddresses[index].to_string();
			g_theRenderer->DrawTextInBox2D( AABB2( 77.f, 90.f - (float) index, 100.f, 92.f - (float) index ), Vector2( 0.f, 0.5f ), client, 1.5f, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );
		}
	}

}