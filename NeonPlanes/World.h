#pragma once

#include "MainMenuState.h"
#include "PauseState.h"
#include "PlayState.h"
#include "GameOverState.h"
#include "HighscoreState.h"
#include "Notifier.h"

class World : public NotifierInterface
{
public:
	World();
	virtual ~World();
	
	GameState* getCurrentState();
	void switchGameState(Uint8 state);

	void action(int parameter);

	bool isSwitchState() const;
	void turnOffSwitchState();
private:
	void addGameState(GameState* state);

	GameState* currentState;
	std::map<int, GameState*> map_states;

	bool switchState;
	bool clearGameWorld;
};
