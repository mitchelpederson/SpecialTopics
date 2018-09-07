#include "Engine/DevConsole/DevConsoleInputBox.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"


void DevConsoleInputBox::Update() {
	m_age += g_masterClock->frame.seconds;

	if (m_age - m_ageAtLastBlink > m_blinkDelay) {
		m_blink = !m_blink;
		m_ageAtLastBlink = m_age;
	}

}


void DevConsoleInputBox::Render() const {
	g_theRenderer->DrawAABB( AABB2(0.f, 0.f, 100.f, m_fontSize), Rgba(100, 100, 100, 100) );
	g_theRenderer->DrawTextInBox2D( AABB2(0.f, 0.f, 100.f, m_fontSize), Vector2(0.f, 0.5f), m_characterStream, m_fontSize, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN );

	if(m_blink) {
		float glyphWidth = m_fontSize * 0.4f;
		g_theRenderer->DrawAABB( AABB2(glyphWidth * m_cursorLocation, 0.f, glyphWidth * (m_cursorLocation + 1.f), m_fontSize), Rgba(255,255,255,200) );
	}
}


void DevConsoleInputBox::SendMessageToDevConsole() {
	if (m_characterStream.length() > 0) {
		DevConsole::GetInstance()->RunCommand(m_characterStream);
		m_cursorLocation = 0;
		m_characterStream = "";
	}
}


void DevConsoleInputBox::InputCharacter(unsigned char character) {
	m_characterStream.insert(m_cursorLocation, 1, character);
	MoveCursorRight();
}


void DevConsoleInputBox::MoveCursorRight() {
	if (m_cursorLocation < 255 && m_cursorLocation < m_characterStream.length()) {
		m_cursorLocation += 1;
	}
}

void DevConsoleInputBox::MoveCursorLeft() {
	if (m_cursorLocation > 0) {
		m_cursorLocation -= 1;
	}
}

void DevConsoleInputBox::Backspace() {
	if (m_cursorLocation > 0) {
		m_cursorLocation -= 1;
		m_characterStream.erase(m_cursorLocation, 1);
	}
}

void DevConsoleInputBox::Delete() {
	if (m_cursorLocation != m_characterStream.length()) {
		m_characterStream.erase(m_cursorLocation, 1);
	}
}


void DevConsoleInputBox::Reset() {
	m_characterStream = "";
	m_cursorLocation = 0;
}

bool DevConsoleInputBox::IsEmpty() {
	return (m_characterStream.length() == 0);
}


void DevConsoleInputBox::SetText( const std::string& text ) {
	m_characterStream = text;
	m_cursorLocation = (int) m_characterStream.size();
}