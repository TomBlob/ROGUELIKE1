#pragma once

#include "Scene.hpp"
#include "Common.hpp"
#include "EntityManager.hpp"

class Scene_Menu : public Scene {

protected:
	std::vector<std::string>	m_levelPaths;
	std::vector<std::string>	m_menuStrings;
	std::string					m_title;
	sf::Text					m_menuText;
	size_t						m_selectedMenuIndex = 0;

	void init();
	void update();
	void sDoAction(const Action& action);
	void onEnd();

public:

	Scene_Menu(GameEngine* gameEngine = nullptr);
	void sRender();
};