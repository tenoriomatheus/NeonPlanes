#include "BlueEnemy.h"

BlueEnemy::BlueEnemy(std::string name, Rectangle* destiny, float base_velocity) : Enemy(name, destiny, base_velocity)
{
	auto patrol = new PatrolBehavior("PatrolBehavior", base_velocity, (Rectangle*)this->getComponent("destiny"), ((Rectangle*)this->getComponent("destiny"))->getSize().x);

	//SPRITES
	this->addSprite(new StaticSprite((Rectangle*)this->getComponent("destiny"), "BlueEnemy.png", "StaticSprite"));

	//BEHAVIORS
	this->addBehavior(new InitialEnemyBehavior(patrol, base_velocity / 2.5, (Rectangle*)this->getComponent("destiny"), ((Rectangle*)this->getComponent("destiny"))->getSize().x, "InitialEnemyBehavior"));

	this->addBehavior(patrol);
	this->getBehavior("PatrolBehavior")->setActive(false);

	this->addBehavior(new ShootBehavior("ShootBehavior", std::bind(&BlueEnemy::shoot, this)));
	this->getBehavior("ShootBehavior")->setActive(false);

	this->addBehavior(new LightWallBehavior("LightWallBehavior", std::bind(&BlueEnemy::useLightWall, this)));
	this->getBehavior("LightWallBehavior")->setActive(false);

	this->addBehavior(new DashBehavior("DashBehavior", base_velocity * 2, (Rectangle*)this->getComponent("destiny"), ((Rectangle*)this->getComponent("destiny"))->getSize().y));
	this->getBehavior("DashBehavior")->setActive(false);

	this->addBehavior(new GoDownBehavior("GoDownBehavior", base_velocity * 1.5, (Rectangle*)this->getComponent("destiny"), ((Rectangle*)this->getComponent("destiny"))->getSize().y));
	this->getBehavior("GoDownBehavior")->setActive(false);
}

BlueEnemy::~BlueEnemy()
{
}

void BlueEnemy::shoot() {
	if (this->testShoot()) {
		this->addPending(new BlueProjectile("BlueProjectile" + std::to_string(objectValue), ((Rectangle*)this->getComponent("destiny"))->getPosition()));
		if (objectValue >= LLONG_MAX) {
			this->objectValue = 0;
		}
		else {
			this->objectValue++;
		}
	}
}

void BlueEnemy::useLightWall() {
	this->addPending(new BlueLight("BlueLight" + std::to_string(objectValue), ((Rectangle*)this->getComponent("destiny"))->getPosition()));
	if (objectValue >= LLONG_MAX) {
		this->objectValue = 0;
	}
	else {
		this->objectValue++;
	}
}