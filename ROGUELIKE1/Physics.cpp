#include "Physics.hpp"
#include "Components.hpp"

sf::Vector2f Physics::GetOverlap(ptr<Entity> a, ptr<Entity> b) {

	auto& aBounding = a->getComponent<CBoundingBox>();
	auto& bBounding = b->getComponent<CBoundingBox>();

	auto& aTransform = a->getComponent<CTransform>();
	auto& bTransform = b->getComponent<CTransform>();

	sf::Vector2f delta(abs(aTransform.pos.x - bTransform.pos.x), abs(aTransform.pos.y - bTransform.pos.y));

	float ox = (aBounding.size.x / 2) + (bBounding.size.x / 2) - delta.x;
	float oy = (aBounding.size.y / 2) + (bBounding.size.y / 2) - delta.y;

	return { ox, oy };
}

sf::Vector2f Physics::GetPreviousOverlap(ptr<Entity> a, ptr<Entity> b)
{
	auto& aBounding = a->getComponent<CBoundingBox>();
	auto& bBounding = b->getComponent<CBoundingBox>();

	auto& aTransform = a->getComponent<CTransform>();
	auto& bTransform = b->getComponent<CTransform>();

	sf::Vector2f delta(abs(aTransform.prevPos.x - bTransform.prevPos.x), abs(aTransform.prevPos.y - bTransform.prevPos.y));

	float ox = (aBounding.size.x / 2) + (bBounding.size.x / 2) - delta.x;
	float oy = (aBounding.size.y / 2) + (bBounding.size.y / 2) - delta.y;

	return { ox, oy };
}

bool Physics::IsCollision(ptr<Entity> a, ptr<Entity> b)
{
	sf::Vector2f overlap(GetOverlap(a,b));
	if (overlap.x > 0 && overlap.y > 0)
		return true;
	return false;
}
