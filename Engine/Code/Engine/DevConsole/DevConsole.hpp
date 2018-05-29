#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/DevConsole/DevConsoleInputBox.hpp"
#include <string.h>
#include <vector>	


struct Message {
	std::string message;
	Rgba color;
};

class DevConsole {
public:
	DevConsole();
	~DevConsole();

	void Update();
	void Render() const;

	void Open();
	void Close();
	void ToggleOpen();
	void RunCommand(std::string commandString);
	static void Clear( const std::string& command );
	static void SaveLog( const std::string& command );


public:
	// I like this as a C function or a static 
	// method so that it is easy for systems to check if the dev
	// console is open.  
	static bool IsOpen(); 

	// Should add a line of coloured text to the output 
	static void Printf( const Rgba& color, const char* format , ...  ); 

	// Same as previous, be defaults to a color visible easily on your console
	static void Printf( const char* format , ...  ); // TO-DO: Add Printf style formatting support
	
	static DevConsole* GetInstance();


private:

	void AddMessage( const char* message, const Rgba& color );

	void ProcessTypingInput();
	void CheckKey( int keyCode, char asciiUpper, char asciiLower );

	bool m_isOpen = false;
	static DevConsole* m_instance;
	Camera* m_camera = nullptr;
	float m_fontSize = 2.5f;
	BitmapFont* m_currentFont = nullptr;
	std::vector<Message> m_messages;

	DevConsoleInputBox m_inputBox;

	int m_currentCommandHistoryIndex = -1;
};