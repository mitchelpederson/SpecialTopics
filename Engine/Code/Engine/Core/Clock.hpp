#pragma once
#include <stdint.h>
#include <vector>


//------------------------------------------------------------------------
//------------------------------------------------------------------------
struct time_unit 
{
	uint64_t hpc; 
	double hp_seconds;  // high precision seconds

	float seconds;      // convenience float seconds
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class Clock 
{
public:
	Clock( Clock *parent = nullptr );
	~Clock(); // make sure to cleanup the heirachy

			  // resets the clock - everything should be zeroes out
			  // and the reference hpc variables set 
			  // to the current hpc.
	void Reset(); 

	// used on a root clock
	// calcualtes elapsed time since last call
	// and calls advance.
	void BeginFrame(); 

	// advance the clock by the given hpc
	void Advance( uint64_t const hpc ); 

	// adds a clock as a child to this clock
	void AddChild( Clock *child ); 

	// returns time since this clock 
	// using the current hpc, and our start hpc
	// to get the most up-to-date time.
	double GetCurrentTimeSeconds() const; 

	// manipulation
	void SetPaused( bool paused ); 
	void SetScale( float scale ); 
	

	// add whatever other convenience methods you may want; 
	// ...

	bool IsPaused() const;

public:
	// I don't use "m_" here as 
	// the intended usage is to be able to go 
	// clock->total.seconds -or- clock->frame.seconds; 
	// these are only updated at begin_frame.
	time_unit frame; 
	time_unit total; 

private:
	// local data we need to track for the clock to work; 
	uint64_t m_startHPC = 0;		// hpc when the clock was last reset (made)
	uint64_t m_lastFrameHPC = 0;	// hpc during last begin_frame call

	double m_timeScale = 1.0; 
	unsigned int m_frameCount = 0;        // if you're interested in number of frames your game has processes
	bool m_paused = false;          

	// For the hierarchy 
	Clock *m_parent;
	std::vector<Clock*> m_children; 
}; 


//------------------------------------------------------------------------
// C Funtions
//------------------------------------------------------------------------
// could calso be static Clock* Clock::GetMaster(); 
Clock* GetMasterClock(); 

// convenience - calls begin frame on the master clock
void ClockSystemBeginFrame();

// I now move this here - as this now refers to the master clock
// who is keeping track of the starting reference point. 
double GetCurrentTimeSinceStart(); 

// I usually also add convenience functions so I'm not constantly fetching the master clock, like
// float GetDeltaTime()
// float GetFrameStartTime();
