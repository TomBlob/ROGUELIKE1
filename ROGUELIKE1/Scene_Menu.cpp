#include "Scene_Menu.hpp"
#include "Scene_Settings.hpp"
#include "Scene_Floor.hpp"
#include "Common.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Components.hpp"
#include "Action.hpp"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine) {
	init();
}

void Scene_Menu::init() {

	if (!m_game->getMusicInitialized()) {
		m_game->setupMusic("music/menuMusic.mp3", true);
	}

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::E, "SELECT");

	m_title = "ROGUELIKE";
	m_menuStrings.push_back("Settings");
	m_menuStrings.push_back("Choose Class");
	m_menuStrings.push_back("Start");
	m_menuStrings.push_back("Quit");

	m_menuText.setFont(m_game->assets().getFont("Bookos"));
	m_menuText.setCharacterSize(128);

	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.0f, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);
}

void Scene_Menu::update() {
	sRender();
	m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "UP") {
			if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
			else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
		}
		else if (action.name() == "DOWN") {
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "SELECT") {

			switch (m_selectedMenuIndex) {
			case 0:
				m_game->changeScene("SETTINGS", std::make_shared<Scene_Settings>(m_game));
				break;

			case 1:
				//m_game->changeScene("CLASS", std::make_shared<Scene_Class>(m_game));
				break;

			case 2:
			{
				std::string title = "FLOOR" + std::to_string(m_game->getCurrentFloor());
				m_game->changeScene(title, std::make_shared<Scene_Floor>(m_game, m_game->generateFloor(m_game->getCurrentFloor())));
				break;
			}
			case 3:
				onEnd();
				break;
			}
		}
	}
}

void Scene_Menu::sRender() {
	m_game->window().clear(sf::Color(50, 50, 150));

	m_menuText.setString(m_title);
	m_menuText.setCharacterSize(32);
	m_menuText.setFillColor(sf::Color::Black);
	m_menuText.setPosition(m_game->window().getSize().x / 2 - m_menuText.getCharacterSize() * 3, 20);

	m_game->window().draw(m_menuText);

	for (int i = 0; i < m_menuStrings.size(); i++) {
		m_menuText.setString(m_menuStrings[i]);
		m_menuText.setCharacterSize(26);
		if (i == m_selectedMenuIndex) {
			m_menuText.setFillColor(sf::Color::Green);
		}
		else {
			m_menuText.setFillColor(sf::Color::Black);
		}
		m_menuText.setPosition(m_game->window().getSize().x / 6, m_game->window().getSize().y / 5 + i * 60);

		m_game->window().draw(m_menuText);
	}


	m_game->window().display();
}

void Scene_Menu::onEnd() {

	m_game->window().close();
}