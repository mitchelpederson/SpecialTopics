#include "Engine/Core/Logger.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include <stdarg.h>
#include <fstream>

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


bool Logger::isRunning = false;
ThreadHandle Logger::threadHandle = nullptr;
ThreadSafeQueue<LogEntry> Logger::m_messageQueue;


//----------------------------------------------------------------------------------------------------------------
void Logger::Startup() {
	isRunning = true;
	threadHandle = CreateNewThread("Logger", LogWorker);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::Shutdown() {
	isRunning = false;
	JoinThread( threadHandle );
}


//----------------------------------------------------------------------------------------------------------------
void Logger::FlushMessages( std::fstream& logFile ) {
	LogEntry entry;
	while (m_messageQueue.Pop(&entry)) {
		logFile << entry.tag << ": " << entry.text << std::endl;
	}
}


//----------------------------------------------------------------------------------------------------------------
void Logger::Printf( const char* text, ... ) {
	// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, text );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, text, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogEntry newEntry;
	newEntry.tag = "Default";
	newEntry.text = textLiteral;
	m_messageQueue.Push(newEntry);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::PrintTaggedf( const char* tag, const char* text, ... ) {
	// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, text );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, text, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogEntry newEntry;
	newEntry.tag = tag;
	newEntry.text = textLiteral;
	m_messageQueue.Push(newEntry);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::LogWorker( void* data ) {

	std::fstream logFile;
	logFile.open( "log.txt", std::ios::out );
	
	while (isRunning) {
		FlushMessages( logFile );
		SleepThread( 10 );
	}
}


//----------------------------------------------------------------------------------------------------------------
void Logger::StopThread() {
	isRunning = false;
}