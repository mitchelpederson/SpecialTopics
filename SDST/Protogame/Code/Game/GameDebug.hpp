#pragma once
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/GameCommon.hpp"

typedef void (*command_cb)( const std::string& command ); 


void DebugPause( const std::string& command ) {
	command;
	g_masterClock->SetPaused(!g_masterClock->IsPaused());
}


void DebugTimeScale( const std::string& command ) {
	std::vector<std::string> tokens = SplitString(command, ' ');
	if (tokens.size() >= 1) {
		float scale = (float) atof(tokens[1].c_str());
		g_masterClock->SetScale(scale);
	}
	else {
		DevConsole::GetInstance()->Printf("Specify a time scale! time_scale <scaleFactor>");
	}
}


void RegisterDebugTimeCommands() {
	void (*pauseCB)( const std::string& command ) = DebugPause;
	void (*timeScaleCB)( const std::string& command ) = DebugTimeScale;
	CommandRegistration::RegisterCommand( "pause", pauseCB, "Pauses the game clock" );
	CommandRegistration::RegisterCommand( "time_scale", timeScaleCB, "Sets the game clock's time scale" );
}