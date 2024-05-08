#pragma once

#include "Common.hpp"
#include "Scene.hpp"
#include "Assets.hpp"

typedef std::map<std::string, ptr<Scene>> SceneMap;

class GameEngine {

protected:
	sf::RenderWindow	m_window;
	Assets				m_assets;
	std::string			m_currentScene;
	SceneMap			m_sceneMap;
	size_t				m_simulationSpeed = 1;
	size_t				m_currentFrames = 0;
	bool				m_running = true;
	bool				m_fullscreen = true;

	void init(const std::string& path);
	void update();

	void sUserInput();

	ptr<Scene> currentScene();
	
public:

	GameEngine(const std::string& path);

	void changeScene(const std::string& sceneName, ptr<Scene> scene, bool endCurrentScene = false);

	void quit();
	void run();
	void toggleFullscreen(bool fullscreen);

	sf::RenderWindow& window();
	const Assets& assets() const;
	bool isRunning();
	bool isFullscreen() const;
};