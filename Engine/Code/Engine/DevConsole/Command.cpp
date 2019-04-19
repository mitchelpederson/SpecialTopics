#include "Engine/DevConsole/Command.hpp"
#include "engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <map>
#include <queue>
#include <string>
#include <fstream>

typedef void (*command_cb)( const std::string& command ); 

std::vector<RegisteredCommand_T> CommandRegistration::m_registeredCommands;
std::vector<std::string> CommandRegistration::m_history;


//----------------------------------------------------------------------------------------------------------------
Command::Command( const std::string& command ) : m_command(command) {
	m_tokens = SplitString(m_command, ' ');
}


//----------------------------------------------------------------------------------------------------------------
Command::Command( Command& command ) : m_command(command.GetString()) {}
Command::~Command() {}


//----------------------------------------------------------------------------------------------------------------
std::string Command::GetString() const {
	return m_command;
}


//----------------------------------------------------------------------------------------------------------------
std::string Command::GetFirstToken() {
	std::vector<std::string> tokens = SplitString(m_command, ' ');

	m_position = (int) tokens[0].size() + 1;
	m_currentArg = 1;

	return tokens[0];
}


//----------------------------------------------------------------------------------------------------------------
void CommandRegistration::RegisterCommand( const std::string& name, command_cb callback, const std::string& help ) {

	for (unsigned int index = 0; index < m_registeredCommands.size(); index++) {
		if (m_registeredCommands[index].command == name) {
			ERROR_RECOVERABLE( "Tried to add command with a duplicate name." );
			return;
		}
	}

	RegisteredCommand_T command;
	command.command = name;
	command.commandCallback = callback;
	command.helpString = help;
	m_registeredCommands.push_back(command);
}


//----------------------------------------------------------------------------------------------------------------
void CommandRegistration::RunCommand( Command command ) {
	m_history.push_back(command.GetString());
	std::string firstToken = command.GetFirstToken();

	for (unsigned int index = 0; index < m_registeredCommands.size(); index++) {
		if (m_registeredCommands[index].command == firstToken) {

				m_registeredCommands[index].commandCallback(command.GetString());
				return;
		}
	}

	std::string output = "Could not find command: ";
	output += firstToken;
	DevConsole::Printf(Rgba(255, 0, 0, 255), output.c_str());
}


//----------------------------------------------------------------------------------------------------------------
bool Command::GetNextInt( int& arg ) {
	arg = -1;

	if (m_tokens.size() > m_currentArg) {
		try {
			size_t s = 0;
			arg = std::stoi(m_tokens[m_currentArg], &s);
			m_position += (int) m_tokens[m_currentArg].size() + 1;
		}
		catch (std::exception except) {
			//DevConsole::Printf(Rgba(255, 0, 0, 255), "The first argument must be an integer");
			return false;
		}
	}
	else {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "There is no argument, please type a number after printint");
		return false;
	}

	if (arg == -1) {
		return false;
	}

	//DevConsole::Printf("Successfully read argument: %d", arg);
	m_currentArg++;
	return true;
}


bool Command::GetNextFloat( float& arg ) {
	arg = -1;

	if (m_tokens.size() > m_currentArg) {
		try {
			size_t s = 0;
			arg = std::stof(m_tokens[m_currentArg], &s);
			m_position += (int) m_tokens[m_currentArg].size() + 1;
		}
		catch (std::exception except) {
			//DevConsole::Printf(Rgba(255, 0, 0, 255), "The first argument must be an float");
			return false;
		}
	}
	else {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "There is no argument to read!");
		return false;
	}

	if (arg == -1) {
		return false;
	}

	//DevConsole::Printf("Successfully read argument: %f", arg);
	m_currentArg++;
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool Command::PeekNextInt( int& arg ) const {

	int currentArg = m_currentArg;

	if (m_tokens.size() > currentArg) {
		try {
			arg = std::stoi(m_tokens[ currentArg ]);
		}
		catch (std::exception except) {
			//DevConsole::Printf(Rgba(255, 0, 0, 255), "The first argument must be an integer");
			return false;
		}
	}
	else {
		//DevConsole::Printf(Rgba(255, 0, 0, 255), "There is no argument, please type a number after printint");
		return false;
	}

	//DevConsole::Printf("Successfully read argument: %d", arg);
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool Command::GetNextRgba( Rgba& arg ) {

	while (m_command[m_position] != '(') {
		if (m_command[m_position] == '\0') {
			return false;
		}
		m_position++;
	}

	int closeParens = m_position;
	while (m_command[closeParens] != ')') {
		if (m_command[closeParens] == '\0') {
			return false;
		}
		closeParens++;
	}

	std::string rgba = m_command.substr(m_position + 1, closeParens - m_position);
	DevConsole::Printf(rgba.c_str());

	arg.SetFromText(rgba.c_str());
	m_position = closeParens;
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool Command::GetNextQuotedString( std::string& arg ) {
	while (m_command[m_position] != '\"') {
		if (m_command[m_position] == '\0') {
			return false;
		}
		m_position++;
	}

	int closeQuote = m_position + 1;
	while (m_command[closeQuote] != '\"') {
		if (m_command[closeQuote] == '\0') {
			return false;
		}
		closeQuote++;
	}

	arg = m_command.substr(m_position + 1, closeQuote - m_position - 1);
	m_position = closeQuote;
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool Command::GetNextString( std::string& arg ) {

	unsigned int start = m_position;
	m_position++;

	while (m_command[m_position] != ' ') {
		if (m_command[m_position] == '\0') {
			break;
		}
		m_position++;
	}


	arg = m_command.substr(start, m_position - start);

	while (m_command[m_position] == ' ') {
		if (m_command[m_position] == '\0') {
			break;
		}
		m_position++;
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool Command::PeekNextString( std::string& arg ) const {
	unsigned int start = m_position;
	unsigned int position = m_position + 1;

	while (m_command[ position ] != ' ') {
		if (m_command[ position ] == '\0') {
			break;
		}
		position++;
	}


	arg = m_command.substr(start, position - start);

	while (m_command[ position ] == ' ') {
		if (m_command[ position ] == '\0') {
			break;
		}
		position++;
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
std::string Command::GetRemainingString() const {
	if ( m_position < m_command.size() ) {
		return m_command.substr( m_position, m_command.size() - m_position);
	}
	else {
		return "";
	}
}


//----------------------------------------------------------------------------------------------------------------
Command CommandRegistration::GetPreviousCommand( int index ) {
	return m_history.at(index);
}


//----------------------------------------------------------------------------------------------------------------
void CommandRegistration::SaveCommandHistory() {

	std::fstream* commandHistoryFile = new std::fstream();
	commandHistoryFile->open( "Log/command_history.txt", std::ios::out );

	for ( unsigned int historyIndex = 0; historyIndex < m_history.size(); historyIndex++ ) {
		*commandHistoryFile << m_history[ historyIndex ] << "\n";
	}

	commandHistoryFile->close();
	delete commandHistoryFile;
	commandHistoryFile = nullptr;
}


//----------------------------------------------------------------------------------------------------------------
void CommandRegistration::LoadCommandHistory() {

	std::fstream* commandHistoryFile = new std::fstream();
	commandHistoryFile->open( "Log/command_history.txt", std::ios::in );

	if (commandHistoryFile->is_open()) {
		while ( !commandHistoryFile->eof() ) {
			char buffer[256];
			commandHistoryFile->getline( buffer, 255 );
			m_history.push_back(buffer);
		}
	}

	commandHistoryFile->close();
	delete commandHistoryFile;
	commandHistoryFile = nullptr;
}