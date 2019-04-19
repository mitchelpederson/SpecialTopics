#pragma once
#include "Engine/Math/AABB2.hpp"

#include <string>

class DevConsoleInputBox {
public:

	void Update();
	void Render() const;

	void SendMessageToDevConsole();
	void MoveCursorLeft();
	void MoveCursorRight();
	void InputCharacter(unsigned char character);
	void Backspace();
	void Delete();
	void Reset();
	bool IsEmpty();

	void SetText( const std::string& text );
	const std::string& GetText();
	float GetFontSize();

private:
	std::string m_characterStream = "";
	int m_cursorLocation = 0;

	bool m_blink = true;
	float m_blinkDelay = 0.4f;
	float m_ageAtLastBlink = 0.f;
	float m_age = 0.f;
	float m_fontSize = 20.f;
}; 