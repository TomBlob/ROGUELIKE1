#pragma once

#include "Common.hpp"
#include "Entity.hpp"

namespace Physics {
	sf::Vector2f GetOverlap(ptr<Entity> a, ptr<Entity> b);
	sf::Vector2f GetPreviousOverlap(ptr<Entity> a, ptr<Entity> b);
}