#include "Scene.hpp"
#include "GameEngine.hpp"

Scene::Scene() {

}

Scene::Scene(GameEngine* gameEngine)
	: m_game(gameEngine) {

}

void Scene::setPaused(bool paused) {
	if (paused) {
		m_game->changeSavedMusicVolume(m_game->getMusicVolume());
		if (m_game->getMusicVolume() > 10)
			m_game->changeMusicVolume(10);
	}
	else {
		m_game->changeMusicVolume(m_game->getSavedMusicVolume());
	}
	m_paused = paused;
}

size_t Scene::width() const {
	return m_game->window().getSize().x;
}

size_t Scene::height() const {
	return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const {
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return false;
}

const ActionMap& Scene::getActionMap() const {
	return m_actionMap;
}

void Scene::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
	sf::VertexArray line(sf::LineStrip, 2);

	line[0].position = sf::Vector2f(p1.x, p1.y);
	line[1].position = sf::Vector2f(p2.x, p2.y);

	this->m_game->window().draw(line);
}

void Scene::doAction(const Action& action)
{

}

void Scene::simulate(const size_t frames)
{
}



void Scene::registerAction(int inputKey, const std::string& actionName) {
	m_actionMap[inputKey] = actionName;
}