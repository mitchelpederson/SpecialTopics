#pragma once


#include "Engine/Net/NetSession.hpp"


class NetSessionWidget {

public:
	NetSessionWidget( NetSession* session );


	void Render() const;


public:
	NetSession* m_session = nullptr;
	bool m_shouldDisplay = true;
};