//-----------------------------------------------------------------------------------------------
// App class. Owns the game and engine systems, doesn't do anything else
//

#pragma once

class App {

public:
	App();
	~App();

	void Initialize();
	void Run();
	static void GetMessages( unsigned int msg, size_t wparam, size_t lparam );


private:

	double m_deltaTime = 0.0;
	double m_timeAtStartup = 0.0;
	double m_timeAtStartOfFrame = 0.0;
	double m_timeOfPreviousFrameStart = 0.0;
};