#include "Engine/Core/Logger.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/DevConsole/Command.hpp"

#include <stdarg.h>
#include <fstream>
#include <ctime>
#include <time.h>

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


bool Logger::isRunning = false;
ThreadHandle Logger::threadHandle = nullptr;

std::shared_mutex Logger::m_hooksLock;
std::vector<LogHook> Logger::m_hooks;
std::vector<std::string> Logger::m_tagFilter;
bool Logger::m_isFilterBlacklist = true;
std::fstream* Logger::recentLogFile = nullptr;
std::fstream* Logger::timestampedLogFile = nullptr;
ThreadSafeQueue<LogEntry> Logger::m_messageQueue;


//----------------------------------------------------------------------------------------------------------------
void Logger::Startup() {

	recentLogFile = new std::fstream();
	recentLogFile->open( "Log/log.txt", std::ios::out );

	std::time_t currentTime = std::time(nullptr);
	std::tm timeComponents;
	localtime_s(&timeComponents, &currentTime);
	std::string timeStamp	= "Log/log_"
		+ std::to_string(timeComponents.tm_year + 1900) 
		+ "-"
		+ std::to_string(timeComponents.tm_mon)
		+ "-"
		+ std::to_string(timeComponents.tm_mday)
		+ "_"
		+ std::to_string(timeComponents.tm_hour)
		+ "-"
		+ std::to_string(timeComponents.tm_min)
		+ "-"
		+ std::to_string(timeComponents.tm_sec)
		+ ".txt";

	timestampedLogFile = new std::fstream();
	timestampedLogFile->open( timeStamp.c_str(), std::ios::out );


	isRunning = true;
	threadHandle = CreateNewThread("Logger", LogWorker);

	CommandRegistration::RegisterCommand("log_flush_test", LogFlushTest, "Immediately prints a message to the log" );
	CommandRegistration::RegisterCommand("log_add_filter", AddTagToFilter, "Adds a new tag to the current filter" );
	CommandRegistration::RegisterCommand("log_remove_filter", RemoveTagFromFilter, "Removes a tag from the filter" );
	CommandRegistration::RegisterCommand("log_hide_all", HideAllLogs, "Clears the log filter and sets it to whitelist mode" );
	CommandRegistration::RegisterCommand("log_show_all", EnableAllLogs, "Clears the log filter and sets it to blacklist mode" );
	CommandRegistration::RegisterCommand("log_test", LogTest, "Log stress test" );

	AddHook(WriteToFile, recentLogFile);
	AddHook(WriteToFile, timestampedLogFile);
	AddHook(OutputToOutputWindow, nullptr);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::Shutdown() {
	isRunning = false;
	JoinThread( threadHandle );
}


//----------------------------------------------------------------------------------------------------------------
void Logger::FlushMessages() {
	LogEntry entry;

	while (m_messageQueue.Pop(&entry)) {

		// Do filter removes so we capture that console command
		if (entry.tag == "filterremove") {
			for (unsigned int i = 0; i < m_tagFilter.size(); i++) {
				if (entry.text == m_tagFilter[i]) {
					m_tagFilter[i] = m_tagFilter[m_tagFilter.size() - 1];
					m_tagFilter.pop_back();
					break;
				}
			}
		}


		// Find if tag is in the filter
		bool isTagInFilter = false;
		for (unsigned int i = 0; i < m_tagFilter.size(); i++) {
			if (entry.tag == m_tagFilter[i]) {
				isTagInFilter = true;
			}
		}

		// if it's a blacklist, we do not want to process if the tag was found.
		// if it's a whitelist, we want to process only if it was found
		if (isTagInFilter != m_isFilterBlacklist) {
			for (unsigned int i = 0; i < m_hooks.size(); i++) {
				m_hooks[i].callback(&entry, m_hooks[i].arguments);
			}
		}

		// Now add new filter so that we can capture the command that issued it
		if (entry.tag == "filteradd") {
			m_tagFilter.push_back(entry.text);
		}
	}

}


//----------------------------------------------------------------------------------------------------------------
void Logger::LogFlushTest( const std::string& command ) {
	Logger::Printf("LogFlushTest");
	FlushMessages();
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
	newEntry.time = g_masterClock->total.seconds;
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
	newEntry.time = g_masterClock->total.seconds;
	m_messageQueue.Push(newEntry);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::Warningf( const char* text, ... ) {
	// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];

	va_list variableArgumentList;
	va_start( variableArgumentList, text );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, text, variableArgumentList );	
	va_end( variableArgumentList );

	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogEntry newEntry;
	newEntry.tag = "Warning";
	newEntry.text = textLiteral;
	newEntry.time = g_masterClock->total.seconds;
	m_messageQueue.Push(newEntry);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::Errorf( const char* text, ... ) {
	// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];

	va_list variableArgumentList;
	va_start( variableArgumentList, text );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, text, variableArgumentList );	
	va_end( variableArgumentList );

	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	LogEntry newEntry;
	newEntry.tag = "Error";
	newEntry.text = textLiteral;
	newEntry.time = g_masterClock->total.seconds;
	m_messageQueue.Push(newEntry);
}


//----------------------------------------------------------------------------------------------------------------
void Logger::LogWorker( void* data ) {

	while (isRunning) {
		FlushMessages();
		SleepThread( 30 );
	}
}


//----------------------------------------------------------------------------------------------------------------
void Logger::StopThread() {
	isRunning = false;
}


//----------------------------------------------------------------------------------------------------------------
void Logger::AddHook( logger_hook_cb hookCB, void* arguments ) {
	m_hooksLock.lock();

	LogHook hook;
	hook.callback = hookCB;
	hook.arguments = arguments;
	m_hooks.push_back(hook);

	m_hooksLock.unlock();
}


//----------------------------------------------------------------------------------------------------------------
void Logger::WriteToFile( const LogEntry* entry, void* filePtr ) {
	std::fstream* file = (std::fstream*) filePtr;
	*file << "[" << entry->time << "] " << entry->tag << ": " << entry->text << std::endl;
}


//----------------------------------------------------------------------------------------------------------------
void Logger::OutputToOutputWindow( const LogEntry* entry, void* arg ) {
	DebuggerPrintf("%s: %s\n", entry->tag.c_str(), entry->text.c_str());
}


//----------------------------------------------------------------------------------------------------------------
void Logger::SetTagFilterMode( bool isBlacklist ) {
	m_isFilterBlacklist = isBlacklist;
}


//----------------------------------------------------------------------------------------------------------------
void Logger::AddTagToFilter( const std::string& tag ) {
	std::vector<std::string> tokens = SplitString(tag, ' ');
	if (tokens.size() >= 2) {
		PrintTaggedf("filteradd", tokens[1].c_str());
	}
}


//----------------------------------------------------------------------------------------------------------------
void Logger::RemoveTagFromFilter( const std::string& tag ) {
	std::vector<std::string> tokens = SplitString(tag, ' ');
	if (tokens.size() >= 2) {
		PrintTaggedf("filterremove", tokens[1].c_str());
	}
}


//----------------------------------------------------------------------------------------------------------------
void Logger::HideAllLogs( const std::string& command ) {
	m_isFilterBlacklist = false;
	m_tagFilter.clear();
}


//----------------------------------------------------------------------------------------------------------------
void Logger::EnableAllLogs( const std::string& command ) {
	m_isFilterBlacklist = true;
	m_tagFilter.clear();
}


//----------------------------------------------------------------------------------------------------------------
void Logger::LogTestWorker( void* logTestArgs ) {

	LogTestData* data = (LogTestData*) logTestArgs;

	std::fstream file;
	file.open( data->fileName, std::ios::in );

	std::string line;
	unsigned int lineCount = 0;
	while(std::getline(file, line)) {
		PrintTaggedf("LogTest", "[%u:%u] %s", data->threadCount, lineCount, line.c_str());
		lineCount++;
	}

	delete data;
}


//----------------------------------------------------------------------------------------------------------------
void Logger::LogTest( const std::string& command ) {

	std::vector<std::string> commandTokens = SplitString(command, ' ');
	std::vector<ThreadHandle> handles;
	if (commandTokens.size() >= 3) {
		int threadCount = stoi(commandTokens[2]);
		for (int i = 0; i < threadCount; i++) {
			LogTestData* data = new LogTestData();
			data->fileName = commandTokens[1].c_str();
			data->threadCount = i;
			handles.push_back( CreateNewThread( "test", LogTestWorker, data ) );
		}
	}

	for (unsigned int i = 0; i < handles.size(); i++) {
		JoinThread( handles[i] );
	}
}