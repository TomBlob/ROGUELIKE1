#pragma once

#include "Common.hpp"
#include "Assets.hpp"
#include "Animation.hpp"

class Component {
public:
	bool has = false;
};

class CTransform : public Component {

public:
	sf::Vector2f	prevPos = { 0.0, 0.0 };
	sf::Vector2f	pos = { 0.0, 0.0 };
	sf::Vector2f	scale = { 1.0, 1.0 };
	sf::Vector2f	velocity = { 0.0, 0.0 };
	double			angle = 0;
	float			speed = 0;

	CTransform() {}
	CTransform(const sf::Vector2f& p)
		: pos(p) {}
	CTransform(const sf::Vector2f& p, const sf::Vector2f& sp, const sf::Vector2f& sc, float a)
		: pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}
};

class CLifespan : public Component {
public:
	bool hit = false;
	int lifeSpan = 0;
	int frameCreated = 0;
	CLifespan(int duration, int frame)
		: lifeSpan(duration), frameCreated(frame) {}
};

class CInput : public Component {
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	int lastBulletFrame = 5000;

	CInput() {}
};

class CBoundingBox : public Component {
public:
	sf::Vector2f size;
	sf::Vector2f halfSize;
	CBoundingBox() {}
	CBoundingBox(const sf::Vector2f& s)
		: size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;

	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

class CGravity : public Component {
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component {
public:
	std::string state = "jumping";
	CState() {}
	CState(const std::string& s) : state(s) {}
};

class CRectangle : public Component {

public :
	sf::RectangleShape	rectangle;
	float				transparency = 0;

	CRectangle() {}
	CRectangle(sf::Vector2f xy, sf::Color fillColor, sf::Color outlineColor, int thickness)
		: rectangle(xy) {

		rectangle.setFillColor(fillColor);
		rectangle.setOutlineColor(outlineColor);
		rectangle.setOutlineThickness(thickness);
		rectangle.setOrigin(xy.x / 2.0f, xy.y / 2.0f);
	}
};