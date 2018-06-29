#pragma once
#include "Engine/Core/Stopwatch.hpp"


class GameState {

public:

	GameState();

	virtual void OnEnter();
	virtual void OnBeginExit();
	bool IsReadyToExit();

	virtual void Update();
	virtual void Render() const;

	bool IsFading() const;

	float GetDeltaTime() const;
	float GetElapsedTime() const;


protected:
	void BeginFadeIn( float duration );
	void BeginFadeOut( float duration );
	void UpdateFade();
	void RenderFade() const;
	bool m_readyToExit = false;


private:
	float m_fadeInLength = 0.2f;
	float m_fadeOutLength = 0.2f;
	Clock* m_stateClock = nullptr;

	Stopwatch m_fadeStopwatch;

	bool m_isFading = false;
	bool m_isFadeIn = false;

	GameState* m_nextState = nullptr;
};