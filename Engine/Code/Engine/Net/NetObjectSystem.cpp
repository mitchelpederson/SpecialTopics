#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetObjectSystem.hpp"


//----------------------------------------------------------------------------------------------------------------
// NetObjectConnectionView
//----------------------------------------------------------------------------------------------------------------
NetObjectConnectionView::~NetObjectConnectionView() {
	std::list< NetObjectView_T* >::iterator it = objectViews.begin();

	while( it != objectViews.end() ) {
		delete *it;
		it++;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectConnectionView::AddNetObject( NetObject* obj ) {
	NetObjectView_T* objView = new NetObjectView_T();
	objView->lastSentSnapshot = nullptr;
	objView->networkID = (uint16_t) obj->networkID;
	objView->typeID = obj->typeID;
	objView->timeLastSent = NetSession::instance->GetNetTime();

	objectViews.push_back(objView);
	objectViewByNetworkID[ obj->networkID ] = objView;
	objectViewByLocalPtr[ obj->localPtr ] = objView;
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectConnectionView::RemoveNetObject( NetObject* obj ) {
	NetObjectView_T* objView = objectViewByNetworkID[ obj->networkID ];
	objectViewByNetworkID.erase( obj->networkID );
	objectViewByLocalPtr.erase( obj->localPtr );

	std::list< NetObjectView_T* >::iterator it = objectViews.begin();
	while ( it != objectViews.end() ) {

		if ((*it)->networkID == obj->networkID ) {
			objectViews.erase( it );
			break;
		}

		it++;
	}

	delete objView;
}


//----------------------------------------------------------------------------------------------------------------
NetObjectView_T* NetObjectConnectionView::FindOldestView() {

	std::list< NetObjectView_T* >::iterator it = objectViews.begin();

	if ( it == objectViews.end() ) {
		return nullptr;
	}

	double newestTime = NetSession::instance->GetNetTime();
	NetObjectView_T* toReturn = nullptr;


	while ( it != objectViews.end() ) {
		if ( newestTime > (*it)->timeLastSent ) {
			toReturn = *it;
			newestTime = toReturn->timeLastSent;
		} 
		it++;
	}

	return toReturn;
}



//----------------------------------------------------------------------------------------------------------------
// Net Object System
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
NetObjectSystem::NetObjectSystem( NetSession* session ) : session( session ) {

	m_connectionViews.resize( MAX_CLIENTS, nullptr );
	CreateViewForConnection(0);
	for (int i = 1; i < MAX_CLIENTS; i++ ) {
		if ( session->GetConnection( i ) != nullptr ) {
			CreateViewForConnection( i );
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
NetObjectSystem::~NetObjectSystem() {
	for ( unsigned int i = 0; i < m_typeDefinitions.size(); i++ ) {
		if (m_typeDefinitions[i] != nullptr) {
			delete m_typeDefinitions[i];
			m_typeDefinitions[i] = nullptr;
		}
	}

	std::list< NetObject* >::iterator it = m_objects.begin();
	while (it != m_objects.end()) {
		delete *it;
		m_objects.erase(it);
		it = m_objects.begin();
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::RegisterObjectType( NetObjectDef_T* type ) {
	if (type->id >= m_typeDefinitions.size()) {
		m_typeDefinitions.resize( type->id + 1, nullptr );
	}
	m_typeDefinitions[ type->id ] = type;
}


//----------------------------------------------------------------------------------------------------------------
NetObjectDef_T const& NetObjectSystem::GetObjectTypeByID( uint8_t id ) {
	return *m_typeDefinitions[ id ];
}


//----------------------------------------------------------------------------------------------------------------
NetObject* NetObjectSystem::GetObjectByNetID( uint32_t id ) {
	std::list<NetObject*>::iterator it = m_objects.begin();
	while ( it != m_objects.end() ) {
		if ( (*it)->networkID == id ) {
			return *it;
		}
		it++;
	}
	return nullptr;
}


//----------------------------------------------------------------------------------------------------------------
NetObject* NetObjectSystem::GetObjectByLocalPtr( void* ptr ) {
	std::list<NetObject*>::iterator it = m_objects.begin();
	while ( it != m_objects.end() ) {
		if ( (*it)->localPtr == ptr ) {
			return *it;
		}
		it++;
	}
	return nullptr;
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::SyncObject( uint8_t type, void* ptr ) {
	NetObject* obj = new NetObject();
	obj->networkID = FindAvailableNetID();
	obj->localPtr = ptr;
	obj->typeID = type;

	AddNetObjectToLists( obj );

	NetObjectDef_T const& typeDef = GetObjectTypeByID( type );

	typeDef.getSnapshotCB( obj->snapshot, obj->localPtr );

	NetMessage create( NETMSG_OBJECT_CREATE );
	create.WriteValue<uint8_t>( type );
	create.WriteValue<uint16_t>( obj->networkID );
	typeDef.sendCreateCB( &create, ptr );

	session->SendToAllOtherConnections( create );

	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if (m_connectionViews[i] != nullptr ) {
			m_connectionViews[i]->AddNetObject( obj );
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::OnConnectionJoined( NetConnection* conn ) {
	std::list< NetObject*>::iterator it = m_objects.begin();

	while (it != m_objects.end()) {
		NetObjectDef_T const& objType = GetObjectTypeByID( (*it)->typeID );
		NetMessage createMsg( NETMSG_OBJECT_CREATE );

		createMsg.WriteValue<uint8_t>( (*it)->typeID );
		createMsg.WriteValue<uint16_t>( (*it)->networkID );

		objType.sendCreateCB( &createMsg, (*it)->localPtr );
		conn->Send( createMsg );

		it++;
	}

	CreateViewForConnection( conn->GetConnectionIndex() );
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::UnsyncObject( void* ptr ) {
	NetObject* obj = m_localPtrObjectLookup[ptr];
	if ( obj == nullptr ) {
		return;
	}

	NetObjectDef_T const& typeDef = GetObjectTypeByID( obj->typeID );

	NetMessage destroy( NETMSG_OBJECT_DESTROY );
	destroy.WriteValue<uint16_t>( obj->networkID );
	typeDef.sendDestroyCB( &destroy, obj->localPtr );

	session->SendToAllOtherConnections( destroy );
	RemoveNetObjectFromLists( obj );

	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if (m_connectionViews[i] != nullptr ) {
			m_connectionViews[i]->RemoveNetObject( obj );
		}
	}

	delete obj;
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::OnConnectionLeft( NetConnection* conn ) {
	delete m_connectionViews[ conn->GetConnectionIndex() ];
	m_connectionViews[ conn->GetConnectionIndex() ] = nullptr;
}

//----------------------------------------------------------------------------------------------------------------
uint16_t NetObjectSystem::FindAvailableNetID() {
	uint16_t next = m_nextNetID;
	m_nextNetID++;
	return next;
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::AddNetObjectToLists( NetObject* netObj ) {
	m_objects.push_back( netObj );
	m_localPtrObjectLookup[ netObj->localPtr ] = netObj;
	m_netIDObjectLookup[ netObj->networkID ] = netObj;
}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::RemoveNetObjectFromLists( NetObject* netObj ) {
	std::list< NetObject* >::iterator objectIterator = m_objects.begin();
	while ( objectIterator != m_objects.end() ) {
		if ( *objectIterator == netObj ) {
			m_objects.erase( objectIterator );
			break;
		}
		objectIterator++;
	}
	m_localPtrObjectLookup.erase( netObj->localPtr );
	m_netIDObjectLookup.erase( netObj->networkID );
}


//----------------------------------------------------------------------------------------------------------------
// Only called by host - can assume connection index is 0
void NetObjectSystem::UpdateSnapshots() {

	std::list< NetObject* >::iterator objectIterator = m_objects.begin();

	while ( objectIterator != m_objects.end() ) {
		
		NetObjectDef_T const& typeDef = GetObjectTypeByID( (*objectIterator)->typeID );

		if( (*objectIterator)->snapshot != nullptr ) {
			delete (*objectIterator)->snapshot;
		}

		typeDef.getSnapshotCB( (*objectIterator)->snapshot, (*objectIterator)->localPtr );

		objectIterator++;
	}

}


//----------------------------------------------------------------------------------------------------------------
void NetObjectSystem::CreateViewForConnection( int connectionIndex ) {
	NetObjectConnectionView* view = new NetObjectConnectionView();

	std::list< NetObject* >::iterator objectIt = m_objects.begin();

	while ( objectIt != m_objects.end() ) {
		view->AddNetObject( *objectIt );
		objectIt++;
	}

	m_connectionViews[ connectionIndex ] = view;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetObjectSystem::FillPacketWithUpdates( NetPacket* packet, NetConnection* conn ) {
	NetObjectConnectionView* view = m_connectionViews[conn->GetConnectionIndex()];
	NetObjectView_T* oldest = view->FindOldestView();
	//NetObjectView_T* previousView = nullptr;
	uint8_t addedMessages = 0;

	while ( packet->GetWrittenByteCount() < MTU && oldest != nullptr ) {

		NetMessage update( NETMSG_OBJECT_UPDATE );
		NetObjectDef_T const& oldestDef = GetObjectTypeByID( oldest->typeID );
		NetObject* oldestObj = GetObjectByNetID( oldest->networkID );

		if ( oldestObj != nullptr ) {
			update.WriteValue<uint8_t>( oldest->typeID );
			update.WriteValue<uint16_t>( oldest->networkID );
			oldestDef.sendSnapshotCB( &update, oldestObj->snapshot );

			if ( packet->GetWrittenByteCount() + update.GetWrittenByteCount() + 3 < MTU ) {
				packet->WriteMessage( update );
				addedMessages++;
				oldest->timeLastSent = session->GetNetTime();
				oldest->lastSentSnapshot = oldestObj->snapshot;
			} else {
				break;
			}
		}

		else {
			break;
		}
		oldest = view->FindOldestView();
	}
	return addedMessages;
}