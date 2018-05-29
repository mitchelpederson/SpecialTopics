#include "Engine/DevConsole/Command.hpp"
#include "engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <map>
#include <queue>
#include <string>

typedef void (*command_cb)( const std::string& command ); 

static std::map<std::string, command_cb> g_registeredCommands;


std::map<std::string, command_cb> CommandRegistration::m_registeredCommands;
std::vector<std::string> CommandRegistration::m_history;


Command::Command( const std::string& command ) : m_command(command) {
	m_tokens = SplitString(m_command, ' ');
}


Command::Command( Command& command ) : m_command(command.GetString()) {}
Command::~Command() {}

std::string Command::GetString() const {
	return m_command;
}


std::string Command::GetFirstToken() {
	std::vector<std::string> tokens = SplitString(m_command, ' ');

	m_position = (int) tokens[0].length();

	return tokens[0];
}

void CommandRegistration::RegisterCommand( const std::string& name, command_cb callback ) {
	m_registeredCommands[name] = callback;
}


void CommandRegistration::RunCommand( Command command ) {
	m_history.push_back(command.GetString());
	std::string firstToken = command.GetFirstToken();

	std::map<std::string, command_cb>::iterator commandIterator = m_registeredCommands.find(firstToken);

	if (commandIterator == m_registeredCommands.end()) {
		std::string output = "Could not find command: ";
		output += firstToken;
		DevConsole::Printf(Rgba(255, 0, 0, 255), output.c_str());
		return;
	}

	else {
		commandIterator->second(command.GetString());
	}
}


bool Command::GetNextInt( int& arg ) {
	arg = -1;

	if (m_tokens.size() > m_currentArg) {
		try {
			arg = std::stoi(m_tokens[m_currentArg], &(size_t) m_position);
		}
		catch (std::exception except) {
			DevConsole::Printf(Rgba(255, 0, 0, 255), "The first argument must be an integer");
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

	DevConsole::Printf("Successfully read argument: %d", arg);
	m_currentArg++;
	return true;
}


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


Command CommandRegistration::GetPreviousCommand( int index ) {
	return m_history.at(index);
}