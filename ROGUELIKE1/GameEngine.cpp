#include "GameEngine.hpp"
#include "Assets.hpp"
#include "Scene_Menu.hpp"

GameEngine::GameEngine(const std::string& path) {
	init(path);
}

void GameEngine::init(const std::string& path) {
	m_assets.loadFromFile(path);

	m_window.create(sf::VideoMode(1920, 1080), "Game 2", sf::Style::Fullscreen);
	m_window.setFramerateLimit(60);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::update() {
	sUserInput();
	m_sceneMap[m_currentScene]->update();
	m_currentFrames++;
}

ptr<Scene> GameEngine::currentScene() {
	return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning() {
	return m_running && m_window.isOpen();
}

bool GameEngine::isFullscreen() const
{
	return m_fullscreen;
}

sf::RenderWindow& GameEngine::window() {
	return m_window;
}

const Assets& GameEngine::assets() const {
	return m_assets;
}

void GameEngine::run() {
	while (isRunning()) {
		update();
	}
}

void GameEngine::toggleFullscreen(bool fullscreen) {
	m_fullscreen = fullscreen;
}

void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit();
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::U) {
				std::cout << "screenshot saved to " << "test.png" << std::endl;
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);
				if (texture.copyToImage().saveToFile("test.png")) {
					std::cout << "screenshot saved to " << "test.png" << std::endl;
				}
			}
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
	}
}

void GameEngine::changeScene(const std::string& sceneName, ptr<Scene> scene, bool endCurrentScene) {
	if (scene) {
		m_sceneMap[sceneName] = scene;
		m_currentScene = sceneName;
	}
	else {
		if (m_sceneMap.find(sceneName) == m_sceneMap.end()) {
			std::cerr << "Warning, scene does not exist : " << sceneName << std::endl;
			return;
		}
	}

	if (endCurrentScene) {
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}
}

void GameEngine::quit() {
	m_running = false;
}