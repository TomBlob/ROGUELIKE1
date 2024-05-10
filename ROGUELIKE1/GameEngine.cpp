#include "GameEngine.hpp"
#include "Assets.hpp"
#include "Scene_Menu.hpp"

GameEngine::GameEngine(const std::string& path) {
	init(path);
}

void GameEngine::init(const std::string& path) {
	m_assets.loadFromFile(path);

	loadConfigFile("assets/config.txt");

	if (m_fullscreen) {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE", sf::Style::Fullscreen);
	}
	else {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE");
	}

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

void GameEngine::toggleFullscreen() {
	if (m_fullscreen) {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE");
		m_fullscreen = false;
	}
	else {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE", sf::Style::Fullscreen);
		m_fullscreen = true;
	}
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

void GameEngine::loadConfigFile(const std::string path) {
	std::ifstream file(path);
	std::string str;

	while (file.good() && str != " ") {
		file >> str;
		if (str == "Fullscreen") {
			size_t fullscreen;
			file >> fullscreen;
			if (fullscreen == 0) {
				m_fullscreen = false;
			}
			else {
				m_fullscreen = true;
			}
		}
		else if (str == "Resolution") {
			file >> m_width >> m_height;
		}
		else if (str == "MusicVolume") {
			file >> m_musicVolume;
		}
	}
}

void GameEngine::updateConfigFile(std::string path) {
	std::ofstream file(path);

	if (!file.is_open()) {
		std::cerr << "Error opening config file after saving settings" << std::endl;
		return;
	}

	file << "Fullscreen\t" << (m_fullscreen ? 1 : 0) << std::endl;
	file << "Resolution\t" << m_width << "\t" << m_height << std::endl;
	file << "MusicVolume\t" << m_music.getVolume() << std::endl;
}

bool GameEngine::sameResolutions(size_t width) {
	if (m_width != width) return false;
	return true;
}

void GameEngine::changeResolution(size_t width, size_t height) {

	m_width = width;
	m_height = height;

	if (m_fullscreen) {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE", sf::Style::Fullscreen);
	}
	else {
		m_window.create(sf::VideoMode(m_width, m_height), "ROGUELIKE");
	}
}

void GameEngine::quit() {
	m_running = false;
}

void GameEngine::resetMusicInit() {
	m_musicInitialized = false;
}

bool GameEngine::getMusicInitialized() {
	return m_musicInitialized;
}

void GameEngine::setupMusic(std::string path, bool loop) {
	if (!m_music.openFromFile(path)) {
		std::cerr << "Cannot open music from file : " << path << std::endl;
	}
	m_music.setLoop(loop);
	m_music.setVolume(m_musicVolume);
	m_music.play();
	m_musicInitialized = true;
}

void GameEngine::changeMusicVolume(float volume){
	m_music.setVolume(volume);
	m_musicVolume = volume;
}

float GameEngine::getMusicVolume() {
	return m_musicVolume;
}

void GameEngine::toggleMusic() {
	if (m_music.getStatus() == sf::Music::Playing) {
		m_music.pause();
	}
	else {
		m_music.play();
	}
}