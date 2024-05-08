#include "Scene_Settings.hpp"
#include "Scene_Menu.hpp"
#include "Common.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Components.hpp"
#include "Action.hpp"

Scene_Settings::Scene_Settings(GameEngine* gameEngine)
	: Scene(gameEngine) {
	init();
}

void Scene_Settings::init() {
	m_fullscreen = m_game->isFullscreen();
	if (m_game->sameResolutions(m_resolution[0].first)) {
		m_resolutionIndex = 0;
	}
	else if (m_game->sameResolutions(m_resolution[1].first)) {
		m_resolutionIndex = 1;
	}
	else {
		m_resolutionIndex = 2;
	}

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::E, "SELECT");

	m_title = "SETTINGS";
	m_SettingsStrings.push_back("Fullscreen");
	m_SettingsStrings.push_back("Change Resolution");
	m_SettingsStrings.push_back("Apply");
	m_SettingsStrings.push_back("Quit to menu");

	m_SettingsText.setFont(m_game->assets().getFont("Bookos"));
	m_SettingsText.setCharacterSize(128);

	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.0f, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);
}

void Scene_Settings::update() {
	sRender();
	m_entityManager.update();
}

void Scene_Settings::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "UP") {
			if (m_selectedSettingsIndex > 0) { m_selectedSettingsIndex--; }
			else { m_selectedSettingsIndex = m_SettingsStrings.size() - 1; }
		}
		else if (action.name() == "DOWN") {
			m_selectedSettingsIndex = (m_selectedSettingsIndex + 1) % m_SettingsStrings.size();
		}
		else if (action.name() == "SELECT") {
			switch (m_selectedSettingsIndex) {
			case 0:
				m_game->toggleFullscreen();
				break;
			case 1:
				break;
			case 2:
				if (!m_game->sameResolutions(m_resolution[m_resolutionIndex].first)) {
					m_game->changeResolution(m_resolution[m_resolutionIndex].first, m_resolution[m_resolutionIndex].second);
				}
				if (m_game->isFullscreen() != m_fullscreen) {
					m_game->toggleFullscreen();
				}
				m_game->updateConfigFile("assets/config.txt");
				m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
				
				break;

			case 3:
				m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
				break;
			}
		}
		if (m_selectedSettingsIndex == 0) {
			if (action.name() == "LEFT" || action.name() == "RIGHT") {
				m_fullscreen = !m_fullscreen;
			}
		} else if (m_selectedSettingsIndex == 1) {
			if (action.name() == "LEFT") {
				if (m_resolutionIndex > 0) { m_resolutionIndex--; }
				else { m_resolutionIndex = m_resolution.size() - 1; }
			}
			else if (action.name() == "RIGHT") {
				m_resolutionIndex = (m_resolutionIndex + 1) % m_resolution.size();
			}
		}
	}
}

void Scene_Settings::sRender() {
	m_game->window().clear(sf::Color(50, 50, 150));

	m_SettingsText.setString(m_title);
	m_SettingsText.setCharacterSize(32);
	m_SettingsText.setFillColor(sf::Color::Black);
	m_SettingsText.setPosition(m_game->window().getSize().x / 2 - m_SettingsText.getCharacterSize() * 3, 20);

	m_game->window().draw(m_SettingsText);

	for (int i = 0; i < m_SettingsStrings.size(); i++) {
		m_SettingsText.setString(m_SettingsStrings[i]);
		m_SettingsText.setCharacterSize(26);
		if (i == m_selectedSettingsIndex) {
			m_SettingsText.setFillColor(sf::Color::Green);
		}
		else {
			m_SettingsText.setFillColor(sf::Color::Black);
		}
		m_SettingsText.setPosition(m_game->window().getSize().x / 6, m_game->window().getSize().y / 5 + i * 60);

		m_game->window().draw(m_SettingsText);
	}

	// changing resolution text

	m_SettingsText.setString("< " + std::to_string(m_resolution[m_resolutionIndex].first) + ", " +
		std::to_string(m_resolution[m_resolutionIndex].second) + " >");
	m_SettingsText.setCharacterSize(26);

	if (m_selectedSettingsIndex == 1) {
		m_SettingsText.setFillColor(sf::Color::Green);
	}
	else {
		m_SettingsText.setFillColor(sf::Color::Black);
	}
	m_SettingsText.setPosition(m_game->window().getSize().x / 2 + 30, m_game->window().getSize().y / 5 + 60);
	m_game->window().draw(m_SettingsText);



	// changing yes or no fullscreen
	if (m_fullscreen) {
		m_SettingsText.setString("< Yes! >");
	}
	else {
		m_SettingsText.setString("< No! >");
	}
	m_SettingsText.setCharacterSize(26);

	if (m_selectedSettingsIndex == 0) {
		m_SettingsText.setFillColor(sf::Color::Green);
	}
	else {
		m_SettingsText.setFillColor(sf::Color::Black);
	}
	m_SettingsText.setPosition(m_game->window().getSize().x / 2 + 30, m_game->window().getSize().y / 5);
	m_game->window().draw(m_SettingsText);

	m_game->window().display();
}

void Scene_Settings::onEnd() {

	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}