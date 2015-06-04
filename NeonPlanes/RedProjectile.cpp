#include "RedProjectile.h"

RedProjectile::RedProjectile(std::string name, Vector2D pos) : Projectile(name, pos)
{
	this->addComponent(new Rectangle(this->pos, Vector2D(54, 37), "destiny"));

	this->addSprite(new StaticSprite((Rectangle*)this->getComponent("destiny"), "RedProjectile.png", "StaticSprite"));
	this->addBehavior(new MoveBehavior("moveUPBehavior", Vector2D(0, -8), (Rectangle*)this->getComponent("destiny")));
}

RedProjectile::~RedProjectile()
{
}
