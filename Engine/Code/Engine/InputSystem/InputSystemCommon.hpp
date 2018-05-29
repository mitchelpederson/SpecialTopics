#pragma once

struct KeyButtonState {
	bool isPressed;
	bool justPressed;
	bool justReleased;

	KeyButtonState() : 
		isPressed(false),
		justPressed(false),
		justReleased(false) 
	{}
};
