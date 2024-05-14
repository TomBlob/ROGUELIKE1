#pragma once

#include "GameEngine.hpp"
#include "Scene.hpp"
#include "Common.hpp"
#include "Action.hpp"
#include "EntityManager.hpp"

class Scene_Floor : public Scene {

	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED;
		std::string WEAPON;
	};

protected:

	ptr<Entity>			m_player;
	std::string			m_levelPath;
	PlayerConfig		m_playerConfig;
	bool				m_drawTextures = true;
	bool				m_drawCollision = false;
	bool				m_drawGrid = false;
	const sf::Vector2f	m_gridSize = { 64, 64 };
	sf::Text			m_gridText;

	std::vector<std::string>	m_pauseStrings;
	sf::Text					m_pauseText;
	size_t						m_pauseIndex = 0;

	sf::Vector2i		m_currentRoom = { 0, 0 };

	void init(const std::string& levelPath);

	void loadLevel(const std::string& filename);
	void spawnPlayer();
	void update() override;
	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action) override;
	void sAnimation();
	void onEnd() override;
	void drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2);
	void sRender() override;


public:
	Scene_Floor(GameEngine* gameEngine, const std::string& levelPath);

	sf::Vector2f gridToMidPixel(float gridX, float gridY, ptr<Entity> entity);

};