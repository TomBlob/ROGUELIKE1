#pragma once

#include "Scene.hpp"
#include "Common.hpp"
#include "EntityManager.hpp"

class Scene_Settings : public Scene {

protected:
	std::vector <std::pair<size_t, size_t>> m_resolution = { {960, 540}, {1280, 720}, {1920, 1080} };
	size_t						m_resolutionIndex;

	std::vector<std::string>	m_SettingsStrings;
	std::string					m_title;
	sf::Text					m_SettingsText;
	size_t						m_selectedSettingsIndex = 0;
	bool						m_fullscreen;

	void init();
	void update();
	void sDoAction(const Action& action);
	void onEnd();

public:

	Scene_Settings(GameEngine* gameEngine = nullptr);
	void sRender();
};