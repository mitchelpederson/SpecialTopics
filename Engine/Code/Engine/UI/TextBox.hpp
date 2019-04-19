#pragma once
#include "Engine/Math/AABB2.hpp"
#include <string>

class TextBox {
public:

	void Update();
	void Render() const;
	void ProcessTypingInput();
	void CheckKey( int keyCode, char asciiUpper, char asciiLower );

	void SetFocus( bool inFocus );
	void MoveCursorLeft();
	void MoveCursorRight();
	void InputCharacter(unsigned char character);
	void Backspace();
	void Delete();
	void Reset();
	bool IsEmpty();
	bool IsInFocus() const;

	void SetBounds( const AABB2& bounds );
	void SetFontSize( float fontSize );

	void SetText( const std::string& text );
	const std::string& GetText() const;
	float GetFontSize() const;

private:
	std::string m_characterStream = "";
	int m_cursorLocation = 0;

	bool m_hasFocus = false;
	bool m_blink = true;
	float m_blinkDelay = 0.4f;
	float m_ageAtLastBlink = 0.f;
	float m_age = 0.f;
	float m_fontSize = 20.f;
	AABB2 m_bounds = AABB2(0.f, 0.f, 100.f, 100.f);

}; 