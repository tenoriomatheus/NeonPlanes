#pragma once

#include "GameObject.h"
#include "CheckerCollision.h"

class Layer
{
public:
	Layer(std::string name);
	virtual ~Layer();

	void addGameObject(GameObject* obj);
	void addVectorGameObject(std::vector<GameObject*> vector);
	GameObject* getGameObject(std::string obj) const;
	std::vector<GameObject*> getGameObjects();
	std::string getName() const;
	void addPending();
	void removeMultiple(std::vector<GameObject*> objects);
private:
	std::string name;
	std::vector<GameObject*> objects;
	std::map<std::string, GameObject*> map_objects;
};
