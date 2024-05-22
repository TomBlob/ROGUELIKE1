#pragma once

#include "GameEngine.hpp"
#include "Scene.hpp"
#include "Common.hpp"
#include "Action.hpp"
#include "EntityManager.hpp"
#include "Room.hpp"

enum FadeState { None, FadeIn, FadeOut };

class Scene_Floor : public Scene {

	struct PlayerConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED;
		std::string WEAPON;
	};

	struct BugConfig {
		float X, Y, CX, CY, SPEED, MAXSPEED;
		int HP;
		std::string ATTACK;
	};

protected:

	ptr<Entity>					m_player;
	std::string					m_levelPath;
	PlayerConfig				m_playerConfig;
	BugConfig					m_bugConfig;
	bool						m_drawTextures = true;
	bool						m_drawCollision = false;
	bool						m_drawGrid = false;
	const sf::Vector2f			m_gridSize = { 64, 64 };
	sf::Text					m_gridText;

	std::vector<std::string>	m_pauseStrings;
	sf::Text					m_pauseText;
	size_t						m_pauseIndex = 0;

	int							m_roomsSize;
	std::vector<Room*>			m_rooms;
	Room*						m_currentRoom;

	bool						m_roomSwitch = false;
	int							m_roomExit;
	sf::RectangleShape			m_fadeRect;
	FadeState					m_fadeState = None;
	sf::Clock					m_fadeClock;

	void init(const std::string& levelPath);

	void loadLevel(const std::string& filename);
	void spawnPlayer();
	void spawnEnemy();
	void update() override;
	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action) override;
	void sAnimation();
	void onEnd() override;
	void drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2);
	void sRender() override;

	std::string generateFloor(int floorNum);
	//sf::Vector2i generateMultiples(int roomsSize);
	int getOppositeSide(int previousEntry);
	bool simulateRoomPlacement(sf::Vector2i placement);

	void sMoveEntity(sf::Vector2f roomPos, int entry, ptr<Entity> e);

	void cleanRooms();
	void changeRoom(int entry);

	void startFade();
	void updateFade();

public:
	Scene_Floor(GameEngine* gameEngine, int roomsSize);

	sf::Vector2f gridToMidPixel(float gridX, float gridY, ptr<Entity> entity);

};