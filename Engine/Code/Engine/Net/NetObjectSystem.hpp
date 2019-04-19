#pragma once

#include <list>
#include <vector>
#include <map>

class NetSession;
class NetObject;
class NetMessage;

typedef void	(*send_create_cb)( NetMessage* msg, void* obj );		
typedef void*	(*recv_create_cb)( NetMessage* msg );				
typedef void	(*send_destroy_cb)( NetMessage* msg, void* obj );
typedef void	(*recv_destroy_cb)( NetMessage* msg, void* obj );

typedef void	(*get_snapshot_cb)( void*& snapshot, void* obj );
typedef void	(*send_snapshot_cb)( NetMessage* msg, void* snapshot );
typedef void	(*recv_snapshot_cb)( NetMessage* msg, void* snapshot );
typedef void	(*apply_snapshot_cb)( void* snapshot, void* obj, float snapshotAge );


struct NetObjectView_T {
	uint8_t typeID = 0;
	uint16_t networkID = 0;
	uint8_t ownerConnectionID = 0;
	void* lastSentSnapshot = nullptr;
	double timeLastSent = 0.0;
};


class NetObjectConnectionView {
public:
	~NetObjectConnectionView();


public:
	uint8_t connectionIndex;
	std::list< NetObjectView_T* > objectViews;
	std::map< void*, NetObjectView_T* > objectViewByLocalPtr;
	std::map< uint16_t, NetObjectView_T* > objectViewByNetworkID;

	void AddNetObject( NetObject* );
	void RemoveNetObject( NetObject* );
	void UpdateNetObject( NetObject* );
	NetObjectView_T* FindOldestView();

};


struct NetObjectDef_T {
	uint8_t id = 0;

	send_create_cb		sendCreateCB = nullptr;
	recv_create_cb		recvCreateCB = nullptr;
	send_destroy_cb		sendDestroyCB = nullptr;
	recv_destroy_cb		recvDestroyCB = nullptr;

	get_snapshot_cb		getSnapshotCB = nullptr;
	send_snapshot_cb	sendSnapshotCB = nullptr;
	recv_snapshot_cb	recvSnapshotCB = nullptr;
	apply_snapshot_cb	applySnapshotCB = nullptr;
};


class NetObject {

public:
	uint8_t typeID = 0;
	uint16_t networkID = 0;
	void* localPtr = nullptr;
	void* snapshot = nullptr;
};


	
class NetObjectSystem {

public:
	NetObjectSystem( NetSession* session );
	~NetObjectSystem();

	// Object Types
	void RegisterObjectType( NetObjectDef_T* type );
	NetObjectDef_T const& GetObjectTypeByID( uint8_t id );

	// Object Instances
	NetObject* GetObjectByNetID( uint32_t id );
	NetObject* GetObjectByLocalPtr( void* ptr );
	void SyncObject( uint8_t type, void* ptr );
	void UnsyncObject( void* ptr );
	uint16_t FindAvailableNetID();
	void AddNetObjectToLists( NetObject* netObj );
	void RemoveNetObjectFromLists( NetObject* netObj );
	void UpdateSnapshots();
	uint8_t FillPacketWithUpdates( NetPacket* packet, NetConnection* conn );

	// Views
	void CreateViewForConnection( int connectionIndex );

	void OnConnectionJoined( NetConnection* conn );
	void OnConnectionLeft( NetConnection* conn );


public:
	NetSession* session = nullptr;

private:


private:

	uint16_t m_nextNetID = 0;

	std::vector< NetObjectDef_T* > m_typeDefinitions;
	std::vector< NetObjectConnectionView* > m_connectionViews;
	std::list< NetObject* > m_objects;
	std::map< void*, NetObject* > m_localPtrObjectLookup;
	std::map< uint16_t, NetObject* > m_netIDObjectLookup;

};