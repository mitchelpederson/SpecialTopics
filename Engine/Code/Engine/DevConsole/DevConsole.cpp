#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <stdarg.h>
#include <fstream>

DevConsole* DevConsole::m_instance = nullptr;
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//----------------------------------------------------------------------------------------------------------------
void GetInt( const std::string& command ) {
	Command com(command);

	int i = -1;

	if (com.GetNextInt(i)) {
		DevConsole::Printf(Rgba(0, 255, 0, 255), "%d", i);
	}
	else {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "Failed to read int: %s", command.c_str());
	}
}


//----------------------------------------------------------------------------------------------------------------
void Help (const std::string& command ) {

	for (unsigned int i = 0; i < CommandRegistration::m_registeredCommands.size(); i++) {
		for (unsigned int j = 0; j < CommandRegistration::m_registeredCommands.size() - 1; j++) {
			std::string& before = CommandRegistration::m_registeredCommands[j].command;
			std::string& after = CommandRegistration::m_registeredCommands[j+1].command;
			if (before.compare( after ) > 0) {
				std::swap( CommandRegistration::m_registeredCommands[j], CommandRegistration::m_registeredCommands[j+1] );
			}
		}
	}

	for (unsigned int index = 0; index < CommandRegistration::m_registeredCommands.size(); index++) {
		RegisteredCommand_T const& comm = CommandRegistration::m_registeredCommands[index];
		DevConsole::Printf("%s - %s", comm.command.c_str(), comm.helpString.c_str());
	}
}


//----------------------------------------------------------------------------------------------------------------
void EchoWithColor( const std::string& command ) {
	Command com(command);
	Rgba color;
	std::string quote = "";

	if (!com.GetNextRgba(color)) {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "Could not parse RGBA value");
		return;
	}

	if (!com.GetNextQuotedString(quote)) {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "Could not parse quote");
		return;
	}

	DevConsole::Printf(color, quote.c_str());


}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::SaveLog( const std::string& command ) {
	Command com(command);
	std::string fileName = "";

	if (!com.GetNextQuotedString(fileName)) {
		DevConsole::Printf(Rgba(255, 0, 0, 255), "Could not parse file name");
		return;
	}

	std::fstream file;
	file.open(fileName, std::ios::out);
	if (!file.is_open()) {
		DevConsole::Printf("Failed to open file at %s", fileName.c_str());
		return;
	}

	for (int messageIndex = 0; messageIndex < GetInstance()->m_messages.size(); messageIndex++) {
		file << GetInstance()->m_messages[messageIndex].message << std::endl;
	}

	file.close();

	DevConsole::Printf("Saved to %s", fileName.c_str());
}


//----------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole() {
	if (m_instance == nullptr) {
		m_instance = this;
		Logger::AddHook( ProcessLoggerMessages, nullptr );
	}

	m_currentFont = g_theRenderer->CreateOrGetBitmapFont("Wolfenstein");

	CommandRegistration::RegisterCommand("clear", Clear, "Clears the dev console" );
	CommandRegistration::RegisterCommand("help", Help, "Show all help messages" );
	CommandRegistration::RegisterCommand("echo_with_color", EchoWithColor, "Copies this message but in a new color" );
	CommandRegistration::RegisterCommand("save_log", SaveLog, "Saves all console messages to a file" );


	m_camera = new Camera();
	m_camera->m_frameBuffer->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->m_frameBuffer->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
}


//----------------------------------------------------------------------------------------------------------------
DevConsole::~DevConsole() {
	if (m_instance == this) {
		m_instance = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
DevConsole* DevConsole::GetInstance() {
	return m_instance;
}


//----------------------------------------------------------------------------------------------------------------
bool DevConsole::IsOpen() {
	return m_instance->m_isOpen;
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::Update() {

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_TILDE)) {
		ToggleOpen();
	}

	if (m_isOpen == true) {

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F2)) {
			Printf("Added a new message");
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F3)) {
			Printf(Rgba( (unsigned char) GetRandomIntInRange(100, 255), (unsigned char) GetRandomIntInRange(100, 255), (unsigned char) GetRandomIntInRange(100, 255), 255), "Added a new message");
		}


		ProcessTypingInput();

		

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_LEFT)) {
			m_inputBox.MoveCursorLeft();
		}
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_RIGHT)) {
			m_inputBox.MoveCursorRight();
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DELETE)) {
			m_inputBox.Delete();
		}
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_BACKSPACE)) {
			m_inputBox.Backspace();
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ESC)) {
			if (m_inputBox.IsEmpty()) {
				m_isOpen = false;
			}
			else {
				m_inputBox.Reset();
			}
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DOWN)) {
			m_currentCommandHistoryIndex++;

			if (m_currentCommandHistoryIndex >= CommandRegistration::GetCommandHistorySize()) {
				m_currentCommandHistoryIndex = 0;
			}

			if (CommandRegistration::GetCommandHistorySize() > 0) {
				const Command& old = CommandRegistration::GetPreviousCommand(m_currentCommandHistoryIndex);

				m_inputBox.SetText(old.GetString());
			}
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_UP)) {

			m_currentCommandHistoryIndex--;

			if (m_currentCommandHistoryIndex < -1) {
				m_currentCommandHistoryIndex = CommandRegistration::GetCommandHistorySize() - 1;
			}

			if (CommandRegistration::GetCommandHistorySize() > 0 && m_currentCommandHistoryIndex >= 0) {
				const Command& old = CommandRegistration::GetPreviousCommand(m_currentCommandHistoryIndex);

				m_inputBox.SetText(old.GetString());
			}
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER)) {
			m_inputBox.SendMessageToDevConsole();
			m_currentCommandHistoryIndex = -1;
		}

		m_inputBox.Update();
	}
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::CheckKey( int keyCode, char asciiUpper, char asciiLower ) {
	
	if (g_theInputSystem->WasKeyJustPressed((unsigned char) keyCode)) {									
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SHIFT)) {				
			m_inputBox.InputCharacter(asciiUpper);											
		}																				
		else {																			
			m_inputBox.InputCharacter(asciiLower);										
		}																				
	}																									
}		


//----------------------------------------------------------------------------------------------------------------
void DevConsole::ProcessTypingInput() {


	// Check alphabet
	for (int key = 0x41; key < 0x5A; key++) {
		CheckKey(key, (char) key, (char) (key + 0x20));
	}

	// Check 0 through 9
	for (int key = 0x30; key < 0x3A; key++) {
		if (g_theInputSystem->WasKeyJustPressed((unsigned char) key)) {

			if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SHIFT)) {
				switch (key) {
				case 0x30: m_inputBox.InputCharacter( ')' ); break;
				case 0x31: m_inputBox.InputCharacter( '!' ); break;
				case 0x32: m_inputBox.InputCharacter( '@' ); break;
				case 0x33: m_inputBox.InputCharacter( '#' ); break;
				case 0x34: m_inputBox.InputCharacter( '$' ); break;
				case 0x35: m_inputBox.InputCharacter( '%' ); break;
				case 0x36: m_inputBox.InputCharacter( '^' ); break;
				case 0x37: m_inputBox.InputCharacter( '&' ); break;
				case 0x38: m_inputBox.InputCharacter( '*' ); break;
				case 0x39: m_inputBox.InputCharacter( '(' ); break;
				default: break;
				}
			}
			else {
				m_inputBox.InputCharacter( (unsigned char) key );
			}

		}
	}

	// Check other special characters
	CheckKey(InputSystem::KEYBOARD_APOSTROPHE,		'\"',	'\'');
	CheckKey(InputSystem::KEYBOARD_BACKSLASH,		'|',	'\\');
	CheckKey(InputSystem::KEYBOARD_CLOSEBRACKET,	'}',	']');
	CheckKey(InputSystem::KEYBOARD_OPENBRACKET,		'{',	'[');
	CheckKey(InputSystem::KEYBOARD_COMMA,			'<',	',');
	CheckKey(InputSystem::KEYBOARD_PERIOD,			'>',	'.');
	CheckKey(InputSystem::KEYBOARD_FORWARDSLASH,	'?',	'/');
	CheckKey(InputSystem::KEYBOARD_SEMICOLON,		':',	';');
	CheckKey(InputSystem::KEYBOARD_DASH,			'_',	'-');
	CheckKey(InputSystem::KEYBOARD_EQUALS,			'+',	'=');

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_SPACE)) {
		m_inputBox.InputCharacter(' ');
	}

}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::Render() const {
	if (m_isOpen == true) {
		g_theRenderer->SetShader(nullptr);
		g_theRenderer->DisableDepth();
		g_theRenderer->SetCameraToUI();
		//g_theRenderer->SetAlphaBlending();
		g_theRenderer->DrawAABB( AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 170) );		
		
		g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
		for (int messageIndex = (int) m_messages.size() - 1; messageIndex > (signed int) m_messages.size() - 50 && messageIndex >= 0; messageIndex--) {
			AABB2 messageBoxBounds(0.f, (m_messages.size() - messageIndex) * m_fontSize, 100.f, (m_messages.size() - messageIndex + 1.f) * m_fontSize);
			g_theRenderer->DrawTextInBox2D( messageBoxBounds, Vector2(0.f, 0.5f), m_messages[messageIndex].message, m_fontSize, m_messages[messageIndex].color, 0.4f, m_currentFont, TEXT_DRAW_OVERRUN );
		}

		g_theRenderer->SetShader(nullptr);
		m_inputBox.Render();
	}
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::ToggleOpen() {
	m_isOpen = !m_isOpen;
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::Printf( const char* format, ... ) {

		// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	//m_instance->AddMessage(textLiteral, Rgba());
	Logger::PrintTaggedf("Debug", "%s", textLiteral );
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::Printf( const Rgba& color, const char* format, ... ) {

		// Copied from StringUtils
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	//m_instance->AddMessage(textLiteral, color);
	Logger::PrintTaggedf("Debug", "%s", textLiteral );

}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::AddMessage( const char* message, const Rgba& color ) {
	Message m;
	m.message = message;
	m.color = color;
	m_messages.push_back(m);
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::RunCommand(std::string commandString) {
	DevConsole::Printf(commandString.c_str());

	Command command(commandString);
	CommandRegistration::RunCommand(command);
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::Clear( const std::string& command ) {
	GetInstance()->m_messages.clear();
}


//----------------------------------------------------------------------------------------------------------------
void DevConsole::ProcessLoggerMessages( const LogEntry* entry, void* args ) {
	if (entry->tag == "Debug") {
		m_instance->AddMessage(entry->text.c_str(), Rgba());
	}
}
