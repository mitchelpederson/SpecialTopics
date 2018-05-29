#pragma once
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <sstream>
#include "Engine/Core/Rgba.hpp"

typedef void (*command_cb)( const std::string& command ); 


class Command {
public:
	Command( const std::string& command );
	Command( Command& command );

	~Command();

	std::string GetFirstToken();
	std::string GetString() const;

	bool GetNextInt( int& arg );
	bool GetNextFloat( float& arg );
	bool GetNextString( std::string& arg );
	bool GetNextQuotedString( std::string& arg );
	bool GetNextRgba( Rgba& arg );


private:
	std::string m_name;
	std::string m_command;
	std::stringstream m_stream;
	std::vector<std::string> m_tokens;
	int m_position = 0;
	int m_currentArg = 1;
};


class CommandRegistration {

public:
	static void RegisterCommand( const std::string& name, command_cb callback );
	static void RunCommand( Command command );
	static Command GetPreviousCommand( int index );
	static int GetCommandHistorySize() { return (int) m_history.size(); }

private:
	static std::map<std::string, command_cb> m_registeredCommands;
	static std::vector<std::string> m_history;
};



