#pragma once

#include "GameState.h"

class MainMenuState : public GameState
{
public:
	MainMenuState();
	virtual ~MainMenuState();

	void execute_BTN_Z();
	void execute_BTN_SPACE();
	void execute_BTN_ENTER();
	void execute_LEFT();
	void execute_RIGHT();
	void execute_UP();
	void execute_DOWN();
};
