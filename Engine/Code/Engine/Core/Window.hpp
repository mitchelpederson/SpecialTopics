#pragma once
#include <vector>
typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 


class Window
{
public:
	// Do all the window creation work is currently in App
	Window( int width, int height, void* hinstance ); 
	~Window();

	// Register a function callback to the Window.  Any time Windows processing a 
	// message, this callback should forwarded the information, along with the
	// supplied user argument. 
	void RegisterHandler( windows_message_handler_cb cb ); 

	// Allow users to unregster (not needed for this , but I 
	// like having cleanup methods). 
	void UnregisterHandler( windows_message_handler_cb cb ); 

	// This is safely castable to an HWND
	void* GetHandle() const { return m_hwnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	float GetAspectRatio();

	// ** EXTRAS ** //
	// void SetTitle( char const *new_title ); 

	std::vector<windows_message_handler_cb> m_listeners; 

private:
	void* m_hwnd;   
	static Window* s_window;
	int m_width = 800;
	int m_height = 600;
	float m_aspect = 1.3333f;
	const char* m_appName = "Protogame2D";

public:
	static Window* GetInstance();



};