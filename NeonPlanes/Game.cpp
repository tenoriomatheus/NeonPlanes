#include "Game.h"

World *Game::gameWorld = nullptr;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

Game::Game(std::string name, int windows_x, int windows_y, int flag)
{
	this->init = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "Error in initialize SDL" << std::endl;
		this->init = false;
	}

	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
	{
		std::cerr << "Error in initialize SDL" << std::endl;
		this->init = false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		std::cerr << "Error in initialize SDL_IMAGE" << std::endl;
		this->init = false;
	}

	if (TTF_Init() < 0)
	{
		std::cerr << "Error in initialize SDL_TTF" << std::endl;
		this->init = false;
	}

	window = SDL_CreateWindow(name.c_str(), windows_x, windows_y, SCREEN_WIDTH, SCREEN_HEIGHT, flag);
	if (window == nullptr)
	{
		std::cerr << "Error in initialize window" << std::endl;
		this->init = false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr)
	{
		std::cerr << "Error in initialize renderer" << std::endl;
		this->init = false;
	}
}

bool Game::isInitialize() const {
	return this->init;
}

Game::~Game()
{
	this->clear();
}


bool Game::initialize() {
	std::string str = IMAGE_PATH + "icon64.png";
	SDL_SetWindowIcon(window, IMG_Load(str.c_str()));

	this->gameWorld = new World();

	if (this->gameWorld == nullptr) {
		std::cerr << "Error in initialize game world" << std::endl;
		return false;
	}

#if _DEBUG
	//Debug Mode
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

#else
	//Release Mode
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

#endif // !_DEBUG

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	this->lightState = false;
	this->upState = false;
	this->downState = false;
	this->leftState = false;
	this->rightState = false;

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0){}

	return true;
}

void Game::draw() {
	
	SDL_RenderClear(renderer);

	for each (auto layer in this->gameWorld->getCurrentState()->getVectorLayers()) 
	{
		for each (auto object in layer->getGameObjects()) 
		{
			if (object->isActive()) {
				for each (auto sprite in object->getVectorSprites())
				{
					if (sprite->isActive())
						sprite->draw();
				}
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void Game::update() {

	if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
		
		//IA
		for each (auto ai in this->gameWorld->getCurrentState()->getLayer("EnemyAI")->getGameObjects() )
		{
			auto ai2 = (Base_AI*)ai;
			ai2->manageBehaviors();
		}

		//collision
		if (CheckerCollision::containsAirplanes() && CheckerCollision::containsObjects()) {
			CheckerCollision::checkCollisions();
		}

		//level
		if (((Light_HUD*)this->gameWorld->getCurrentState()->getLayer("HUD")->getGameObject("Light_HUD"))->getIsDecrease() == nullptr)
		{
			((Light_HUD*)this->gameWorld->getCurrentState()->getLayer("HUD")->getGameObject("Light_HUD"))->setDecrease(&this->lightState);
		}
		((PlayState*)this->gameWorld->getCurrentState())->runLevelLogic();
	}

	
	//update behaviors
	for each (auto layer in this->gameWorld->getCurrentState()->getVectorLayers())
	{
		layer->addPending();
		for each (auto object in layer->getGameObjects())
		{
			if (object->isActive()) {
				for each (auto behavior in object->getVectorBaheviors())
				{
					if (behavior->isActive())
						behavior->run();
				}
			}
		}
	}

}

void Game::manageObjects() {
	std::vector<GameObject*> inactivesObjects;
	std::vector<GameObject*> Remove_Interaction;
	std::vector<GameObject*> Remove_EnemyAI;

	if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
		ObjectManager::deleteInactivesObjects();
		
		auto layer_Interaction = this->gameWorld->getCurrentState()->getLayer("Interaction");
		auto layer_EnemyAI = this->gameWorld->getCurrentState()->getLayer("EnemyAI");

		for each (auto object in layer_EnemyAI->getGameObjects())
		{
			if (!object->isActive()) {
				inactivesObjects.push_back(object);
				Remove_EnemyAI.push_back(object);
			}
		}

		layer_EnemyAI->removeMultiple(Remove_EnemyAI);
		Remove_EnemyAI.clear();

		for each (auto object2 in layer_Interaction->getGameObjects())
		{
			if (!object2->isActive()) {
				inactivesObjects.push_back(object2);
				Remove_Interaction.push_back(object2);
			}
		}

		layer_Interaction->removeMultiple(Remove_Interaction);
		Remove_Interaction.clear();

		ObjectManager::addInactiveObjects(inactivesObjects);

		inactivesObjects.clear();
	}
}

bool Game::handlingEvents() {
	SDL_Event e;

	if (SDL_NumJoysticks() == 1) {
		SDL_JoystickEventState(SDL_ENABLE);
		this->joystick = SDL_JoystickOpen(0);
	}

	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
				if (e.key.keysym.sym == SDLK_z) this->lightState = true;
				if (e.key.keysym.sym == SDLK_RETURN) this->gameWorld->getCurrentState()->execute_BTN_ENTER();
				if (e.key.keysym.sym == SDLK_UP) this->upState = true;
				if (e.key.keysym.sym == SDLK_DOWN) this->downState = true;
				if (e.key.keysym.sym == SDLK_LEFT) this->leftState = true;
				if (e.key.keysym.sym == SDLK_RIGHT) this->rightState = true;
			}
			else {
				this->lightState = false;
				this->upState = false;
				this->downState = false;
				this->leftState = false;
				this->rightState = false;
				((PlayState*)this->gameWorld->getMapStates().at(utility::PLAY))->stop(true);

				switch (e.key.keysym.sym) {
				case SDLK_SPACE:
					this->gameWorld->getCurrentState()->execute_BTN_SPACE();
					break;
				case SDLK_z:
					this->gameWorld->getCurrentState()->execute_BTN_Z();
					break;
				case SDLK_RETURN:
					this->gameWorld->getCurrentState()->execute_BTN_ENTER();
					break;
				case SDLK_UP:
					this->gameWorld->getCurrentState()->execute_UP();
					break;
				case SDLK_DOWN:
					this->gameWorld->getCurrentState()->execute_DOWN();
					break;
				case SDLK_LEFT:
					this->gameWorld->getCurrentState()->execute_LEFT();
					break;
				case SDLK_RIGHT:
					this->gameWorld->getCurrentState()->execute_RIGHT();
					break;
				default:
					break;
				}
			}
			break;
		case SDL_KEYUP:
			if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
				if (e.key.keysym.sym == SDLK_SPACE) this->gameWorld->getCurrentState()->execute_BTN_SPACE();
				if (e.key.keysym.sym == SDLK_z) this->lightState = false;
				if (e.key.keysym.sym == SDLK_UP) {
					this->upState = false;
					((PlayState*)this->gameWorld->getCurrentState())->stop(true);
				}
				if (e.key.keysym.sym == SDLK_DOWN) {
					this->downState = false;
					((PlayState*)this->gameWorld->getCurrentState())->stop(true);
				}
				if (e.key.keysym.sym == SDLK_LEFT) {
					this->leftState = false;
					((PlayState*)this->gameWorld->getCurrentState())->stop(false);
				}
				if (e.key.keysym.sym == SDLK_RIGHT) {
					this->rightState = false;
					((PlayState*)this->gameWorld->getCurrentState())->stop(false);
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if (e.jbutton.button == 4) {
				//start
				this->gameWorld->getCurrentState()->execute_BTN_ENTER();
			}
			if (typeid(*this->gameWorld->getCurrentState()) != typeid(PlayState)) {
				if (e.jbutton.button == 10) {
					//btn A
					this->gameWorld->getCurrentState()->execute_BTN_SPACE();
				}
			}
			break;
		case SDL_JOYBUTTONUP:
			if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
				if (e.jbutton.button == 10) {
					//btn A
					this->gameWorld->getCurrentState()->execute_BTN_SPACE();
				}
			}
			break;
		case SDL_JOYAXISMOTION:

			if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState))
			{
				if (e.jaxis.axis == 0) {
					if (e.jaxis.value < -this->deadZone) {
						this->leftState = true;
					}
					else if (e.jaxis.value > this->deadZone) {
						this->rightState = true;
					}
					if ((e.jaxis.value > -this->deadZone) && (e.jaxis.value < this->deadZone)) {
						this->leftState = false;
						this->rightState = false;
						((PlayState*)this->gameWorld->getCurrentState())->stop(false);
					}
				}
				if (e.jaxis.axis == 1) {
					if (e.jaxis.value < -this->deadZone) {
						this->upState = true;
					}
					else if (e.jaxis.value > this->deadZone) {
						this->downState = true;
					}
					if ((e.jaxis.value > -this->deadZone) && (e.jaxis.value < this->deadZone)) {
						this->upState = false;
						this->downState = false;
						((PlayState*)this->gameWorld->getCurrentState())->stop(true);
					}
				}
				if (e.jaxis.axis == 5) {
					//btn RT
					if (e.jaxis.value > this->deadZone) {
						this->lightState = true;
					}
					if (e.jaxis.value < this->deadZone) {
						this->lightState = false;
					}
				}
			}

			else 
			{
				this->lightState = false;
				this->upState = false;
				this->downState = false;
				this->leftState = false;
				this->rightState = false;
				((PlayState*)this->gameWorld->getMapStates().at(utility::PLAY))->stop(true);

				if (e.jaxis.axis == 0) {
					if (e.jaxis.value < -this->deadZone) {
						this->gameWorld->getCurrentState()->execute_LEFT();
					}
					else if (e.jaxis.value > this->deadZone) {
						this->gameWorld->getCurrentState()->execute_RIGHT();
					}
				}
				if (e.jaxis.axis == 1) {
					if (e.jaxis.value < -this->deadZone) {
						this->gameWorld->getCurrentState()->execute_UP();
					}
					else if (e.jaxis.value > this->deadZone) {
						this->gameWorld->getCurrentState()->execute_DOWN();
					}
				}
			}
		break;
		default:
			break;
		}
	}


	if (typeid(*this->gameWorld->getCurrentState()) == typeid(PlayState)) {
		if (this->lightState) {
			this->gameWorld->getCurrentState()->execute_BTN_Z();
		}
		if (this->upState) {
			this->gameWorld->getCurrentState()->execute_UP();
		}
		if (this->downState) {
			this->gameWorld->getCurrentState()->execute_DOWN();
		}
		if (this->leftState) {
			this->gameWorld->getCurrentState()->execute_LEFT();
		}
		if (this->rightState) {
			this->gameWorld->getCurrentState()->execute_RIGHT();
		}
	}

	return true;
}

void Game::clear() {
	delete this->gameWorld;
	this->gameWorld = nullptr;
	
	SDL_JoystickClose(this->joystick);
	this->joystick = nullptr;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = nullptr;
	renderer = nullptr;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

World* Game::getGameWorld() {
	return gameWorld;
}