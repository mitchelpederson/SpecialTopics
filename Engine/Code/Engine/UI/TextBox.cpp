#include "Engine/UI/TextBox.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

//----------------------------------------------------------------------------------------------------------------
void TextBox::Update() {
	if ( m_hasFocus ) {

		ProcessTypingInput();

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_LEFT)) {
			MoveCursorLeft();
		}
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_RIGHT)) {
			MoveCursorRight();
		}

		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DELETE)) {
			Delete();
		}
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_BACKSPACE)) {
			Backspace();
		}

	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::Render() const {

	unsigned char alpha = 100;
	if ( !m_hasFocus ) {
		alpha = 50;
	}

	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));
	g_theRenderer->DrawAABB( m_bounds, Rgba(100, 100, 100, alpha) );
	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
	g_theRenderer->DrawTextInBox2D( m_bounds, Vector2(0.f, 0.5f), m_characterStream, m_fontSize, Rgba(255,255,255, alpha+155), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );

	if(m_blink && m_hasFocus) {
		float glyphWidth = m_fontSize;
		g_theRenderer->DrawAABB( AABB2((glyphWidth * m_cursorLocation) + m_bounds.mins.x, m_bounds.mins.y, (glyphWidth * (m_cursorLocation + 1.f)) + m_bounds.mins.x, m_fontSize + m_bounds.mins.y), Rgba(255,255,255,200) );
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::ProcessTypingInput() {

	// Check alphabet
	for (int key = 0x41; key < 0x5A; key++) {
		CheckKey(key, (char) key, (char) (key + 0x20));
	}

	// Check 0 through 9
	for (int key = 0x30; key < 0x3A; key++) {
		if (g_theInputSystem->WasKeyJustPressed((unsigned char) key)) {

			if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SHIFT)) {
				switch (key) {
				case 0x30: InputCharacter( ')' ); break;
				case 0x31: InputCharacter( '!' ); break;
				case 0x32: InputCharacter( '@' ); break;
				case 0x33: InputCharacter( '#' ); break;
				case 0x34: InputCharacter( '$' ); break;
				case 0x35: InputCharacter( '%' ); break;
				case 0x36: InputCharacter( '^' ); break;
				case 0x37: InputCharacter( '&' ); break;
				case 0x38: InputCharacter( '*' ); break;
				case 0x39: InputCharacter( '(' ); break;
				default: break;
				}
			}
			else {
				InputCharacter( (unsigned char) key );
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
		InputCharacter(' ');
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::CheckKey( int keyCode, char asciiUpper, char asciiLower ) {
	if (g_theInputSystem->WasKeyJustPressed((unsigned char) keyCode)) {									
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SHIFT)) {				
			InputCharacter(asciiUpper);											
		}																				
		else {																			
			InputCharacter(asciiLower);										
		}																				
	}	
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::InputCharacter( unsigned char character ) {
	m_characterStream.insert(m_cursorLocation, 1, character);
	MoveCursorRight();
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::SetFocus( bool inFocus ) {
	m_hasFocus = inFocus;
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::MoveCursorLeft() {
	if (m_cursorLocation > 0) {
		m_cursorLocation -= 1;
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::MoveCursorRight() {
	if (m_cursorLocation < 255 && m_cursorLocation < m_characterStream.length()) {
		m_cursorLocation += 1;
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::Backspace() {
	if (m_cursorLocation > 0) {
		m_cursorLocation -= 1;
		m_characterStream.erase(m_cursorLocation, 1);
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::Delete() {
	if (m_cursorLocation != m_characterStream.length()) {
		m_characterStream.erase(m_cursorLocation, 1);
	}
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::Reset() {
	m_characterStream = "";
	m_cursorLocation = 0;
}


//----------------------------------------------------------------------------------------------------------------
bool TextBox::IsEmpty() {
	return (m_characterStream.length() == 0);
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::SetText( const std::string& text ) {
	m_characterStream = text;
	m_cursorLocation = (int) m_characterStream.size();
}


//----------------------------------------------------------------------------------------------------------------
const std::string& TextBox::GetText() const {
	return m_characterStream;
}


//----------------------------------------------------------------------------------------------------------------
float TextBox::GetFontSize() const {
	return m_fontSize;
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::SetFontSize( float fontSize ) {
	m_fontSize = fontSize;
}


//----------------------------------------------------------------------------------------------------------------
void TextBox::SetBounds( const AABB2& bounds ) {
	m_bounds = bounds;
}


//----------------------------------------------------------------------------------------------------------------
bool TextBox::IsInFocus() const {
	return m_hasFocus;
}