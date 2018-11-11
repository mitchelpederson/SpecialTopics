#include "Engine/DevConsole/NetSessionWidget.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
NetSessionWidget::NetSessionWidget( NetSession* session ) : m_session( session ) {

}


//----------------------------------------------------------------------------------------------------------------
void NetSessionWidget::Render() const {
	if (m_shouldDisplay) {
		Renderer* r = g_theRenderer;
		BitmapFont* font = r->CreateOrGetBitmapFont("Wolfenstein");

		r->DrawAABB( AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 200) );
		r->DrawText2D( Vector2(0.f, 96.f), "Session Information", 4.f, Rgba(255, 255, 255, 255), 0.5f, font );

		std::string simInfo = Stringf( "sim lag: %fs-%fs sim loss: %*f", m_session->GetSimLatency().min, m_session->GetSimLatency().max, 8, m_session->GetSimLossRate() );
		r->DrawText2D( Vector2(0.f, 94.f), simInfo, 2.f, Rgba(180, 180, 180, 255), 0.5f, font );


		// idx, address, rtt, loss, last recv time, last sent time, send ack, recv ack, recv bits
		std::string connectionFormat = "%-*u %-*s %-*f %-*f %-*f %-*f %-*u %-*u %-*s %-*u";
		std::string header = Stringf( "%-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s", 
			4, "idx",
			18, "address",
			9, "rtt",
			9, "loss",
			9, "lastrecv",
			9, "lastsend",
			9, "sentack",
			9, "recvack",
			16, "recv bits",
			8, "uncfmed");

		r->DrawTextInBox2D( AABB2( 0.f, 92.f, 100.f, 94.f ), Vector2( 0.f, 0.5f ), header, 1.8f, Rgba(180, 180, 180, 255), 0.5f, font, TEXT_DRAW_OVERRUN );

		int numConnections = 0;
		for ( unsigned int i = 0; i < MAX_CLIENTS; i++ ) {
			if ( m_session->GetConnection(i) != nullptr ) {
				
				numConnections++;
				NetConnection* conn = m_session->GetConnection(i);
				std::string connectionString = Stringf( connectionFormat.c_str(), 
					4, i,
					18, conn->GetAddressAsString().c_str(),
					9, conn->GetRTT(),
					9, conn->GetLoss(),
					9, g_masterClock->total.seconds - conn->GetTimeAtLastReceive(),
					9, g_masterClock->total.seconds - conn->GetTimeAtLastSend(),
					9, conn->GetNextAckToSend() - 1,
					9, conn->GetLastRecvdAck(),
					16, PrintUint16Binary(conn->GetPreviousRecvdAckBitfield()).c_str(),
					8, conn->GetNumUnconfirmedReliables());

				r->DrawTextInBox2D( AABB2( 0.f, 92.f - (numConnections * 1.8f), 100.f, 94.f - (numConnections * 1.8f) ), Vector2( 0.f, 0.5f ), connectionString, 1.8f, Rgba(180, 180, 180, 255), 0.5f, font, TEXT_DRAW_OVERRUN );

			}
		}
	}

}