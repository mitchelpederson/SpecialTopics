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
	float w = (float) Window::GetInstance()->GetWidth();
	float h = (float) Window::GetInstance()->GetHeight();
	float fontSize = 30.f;

	// Print our local IP and our current state
	std::string myIP = "Local IP: " + NetAddress_T::GetLocal(29283).to_string();
	g_theRenderer->DrawTextInBox2D( AABB2( (w * 0.25f) * 3.f, h - 30.f, w, h ), Vector2(0.f, 0.5f), header.c_str(), fontSize, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );	
	g_theRenderer->DrawTextInBox2D( AABB2( (w * 0.25f) * 3.f, h - 60.f, w, h - 30.f ), Vector2(0.f, 0.5f), myIP, 1.5f, Rgba(), fontSize, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );

	// If hosting, show all the clients connected to us
	if ( RemoteCommandService::IsHost() ) {
		float innerFontSize = 20.f;

		std::vector<NetAddress_T> clientAddresses = RemoteCommandService::GetClientAddresses();
		for ( unsigned int index = 0; index < clientAddresses.size(); index++ ) {
			std::string client = "[" + std::to_string(index) + "] ";
			client += clientAddresses[index].to_string();
			g_theRenderer->DrawTextInBox2D( AABB2( (w * 0.25f) * 3.f, (h - 110.f) - ((float) index * innerFontSize), 100.f, (h - 90.f) - ((float) index * innerFontSize) ), Vector2( 0.f, 0.5f ), client, innerFontSize, Rgba(), 0.9f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );
		}
	}
	
}