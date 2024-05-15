#include "Scene_Floor.hpp"
#include "Common.hpp"
#include "Physics.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Action.hpp"
#include "Components.hpp"
#include "Scene_Menu.hpp"

Scene_Floor::Scene_Floor(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath) {
	init(m_levelPath);
}

void Scene_Floor::init(const std::string& levelPath) {
	registerAction(sf::Keyboard::Escape, "PAUSE");
	registerAction(sf::Keyboard::Delete, "FORCEQUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::E, "SELECT");

	m_pauseText.setFont(m_game->assets().getFont("Bookos"));
	m_pauseText.setCharacterSize(25);

	m_pauseStrings.push_back("Resume");
	m_pauseStrings.push_back("Abandon Run");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Bookos"));

	loadLevel(levelPath);
}

sf::Vector2f Scene_Floor::gridToMidPixel(float gridX, float gridY, ptr<Entity> entity) {

	float x, y;
	x = gridX + entity->getComponent<CAnimation>().animation.getSize().x / 2;
	y = gridY - entity->getComponent<CAnimation>().animation.getSize().y / 2;

	return sf::Vector2f(x, y);
}

void Scene_Floor::loadLevel(const std::string& path) {

	m_entityManager = EntityManager();

	std::ifstream file(path);
	std::string str;

	while (file.good() && str != " ") {

		file >> str;

		if (str == "Tile") {
			std::string type;
			int x, y;

			file >> type >> x >> y;
			auto e = m_entityManager.addEntity("Tile");

			if (type == "GreyBrick") {
				e->addComponent<CAnimation>(m_game->assets().getAnimation(type), true);
				e->addComponent<CTransform>(gridToMidPixel(64 * x, m_game->window().getSize().y - 64 * y, e));
				e->getComponent<CTransform>().scale.x = 64 / e->getComponent<CAnimation>().animation.getSize().x;
				e->getComponent<CTransform>().scale.y = 64 / e->getComponent<CAnimation>().animation.getSize().y;
				e->addComponent<CBoundingBox>(e->getComponent<CAnimation>().animation.getSize());
			}
		}
		else if (str == "Button") {
			std::string type;
			int id;

			file >> type >> id;
			auto e = m_entityManager.addEntity("Button");
			if (type == "PauseButton") {
				e->addComponent<CAnimation>(m_game->assets().getAnimation(type), true);
				e->addComponent<CId>(id);
				e->addComponent<CTransform>();
				e->getComponent<CTransform>().scale.x = 64 / e->getComponent<CAnimation>().animation.getSize().x;
				e->getComponent<CTransform>().scale.y = 64 / e->getComponent<CAnimation>().animation.getSize().y;
			}
		}
		else if (str == "Player") {
			file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED
				>> m_playerConfig.MAXSPEED;
		}
	}
	spawnPlayer();
}

void Scene_Floor::spawnPlayer() {
	
	m_player = m_entityManager.addEntity("Player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyStand"), true);
	m_player->addComponent<CInput>();
	m_player->addComponent<CState>("Standing");
	m_player->addComponent<CTransform>(gridToMidPixel(64 * m_playerConfig.X, m_game->window().getSize().y - 64 * m_playerConfig.Y, m_player));
	m_player->getComponent<CTransform>().scale.x = 64 / m_player->getComponent<CAnimation>().animation.getSize().x;
	m_player->getComponent<CTransform>().scale.y = 64 / m_player->getComponent<CAnimation>().animation.getSize().y;
	m_player->addComponent<CBoundingBox>(sf::Vector2f(m_playerConfig.CX, m_playerConfig.CY));
}

void Scene_Floor::update() {
	m_entityManager.update();

	if (!m_paused) {
		sMovement();
		sLifespan();
		sCollision();
		m_currentFrame++;
	}
	sAnimation();
	sRender();
}

void Scene_Floor::sMovement() {
	auto& pInput = m_player->getComponent<CInput>();
	auto& pState = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();
	auto& currentAnim = m_player->getComponent<CAnimation>();

	sf::Vector2f vel = { 0, 0 };

	if (!(pInput.down || pInput.up || pInput.left || pInput.right)) {
			pState.state = "Standing";
	}

	if (pState.state == "WalkingUp") {
		while (true) {
			if (pInput.up) {
				pState.state = "WalkingUp";
				break;
			}
			if (pInput.down) {
				pState.state = "WalkingDown";
				break;
			}
			if (pInput.left) {
				pState.state = "WalkingLeft";
				break;
			}
			if (pInput.right) {
				pState.state = "WalkingRight";
				break;
			}
			break;
		}
	}
	else if (pState.state == "WalkingLeft") {
		while (true) {
			if (pInput.left) {
				pState.state = "WalkingLeft";
				break;
			}
			if (pInput.up) {
				pState.state = "WalkingUp";
				break;
			}
			if (pInput.down) {
				pState.state = "WalkingDown";
				break;
			}
			if (pInput.right) {
				pState.state = "WalkingRight";
				break;
			}
			break;
		}
	}
	else if (pState.state == "WalkingRight") {
		while (true) {
			if (pInput.right) {
				pState.state = "WalkingRight";
				break;
			}
			if (pInput.up) {
				pState.state = "WalkingUp";
				break;
			}
			if (pInput.down) {
				pState.state = "WalkingDown";
				break;
			}
			if (pInput.left) {
				pState.state = "WalkingLeft";
				break;
			}
			break;
		}
	}
	else {
		while (true) {
			if (pInput.down) {
				pState.state = "WalkingDown";
				break;
			}
			if (pInput.up) {
				pState.state = "WalkingUp";
				break;
			}
			if (pInput.left) {
				pState.state = "WalkingLeft";
				break;
			}
			if (pInput.right) {
				pState.state = "WalkingRight";
				break;
			}
			break;
		}
	}

	if (pInput.up) {
		vel.y -= m_playerConfig.SPEED;
	}
	else {
		if (pTransform.velocity.y < 0)
			vel.y += m_playerConfig.SPEED;
	}
	if (pInput.down) {
		vel.y += m_playerConfig.SPEED;
	}
	else {
		if (pTransform.velocity.y > 0)
			vel.y -= m_playerConfig.SPEED;
	}
	if (pInput.right) {
		vel.x += m_playerConfig.SPEED;
	}
	else {
		if (pTransform.velocity.x > 0)
			vel.x -= m_playerConfig.SPEED;
	}
	if (pInput.left) {
		vel.x -= m_playerConfig.SPEED;
	}
	else {
		if (pTransform.velocity.x < 0)
			vel.x += m_playerConfig.SPEED;
	}

	pTransform.velocity += vel;

	if (pTransform.velocity.y < - m_playerConfig.MAXSPEED) {
		pTransform.velocity.y = - m_playerConfig.MAXSPEED;
	}
	if (pTransform.velocity.y > m_playerConfig.MAXSPEED) {
		pTransform.velocity.y = m_playerConfig.MAXSPEED;
	}
	if (pTransform.velocity.x < - m_playerConfig.MAXSPEED) {
		pTransform.velocity.x = - m_playerConfig.MAXSPEED;
	}
	if (pTransform.velocity.x > m_playerConfig.MAXSPEED) {
		pTransform.velocity.x = m_playerConfig.MAXSPEED;
	}

	pTransform.prevPos = pTransform.pos;
	pTransform.pos += pTransform.velocity;
}

void Scene_Floor::sLifespan() {
	for (auto e : m_entityManager.getEntities()) {
		if (e->hasComponent<CLifespan>()) {
			if (m_currentFrame - e->getComponent<CLifespan>().frameCreated > e->getComponent<CLifespan>().lifeSpan) {
				e->destroy();
			}
		}
	}
}

void Scene_Floor::sCollision() {

}

void Scene_Floor::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "UP") {
			m_player->getComponent<CInput>().up = true;
			if (m_paused && m_pauseIndex > 0) {
				m_pauseIndex--;
			}
		}
		else if (action.name() == "DOWN") {
			m_player->getComponent<CInput>().down = true;
			if (m_paused && m_pauseIndex < m_pauseStrings.size() - 1) {
				m_pauseIndex++;
			}
		}
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "SELECT") {
			if (m_paused) {
				if (m_pauseIndex == 0) {
					setPaused(false);
				}
				else {
					m_game->resetMusicInit();
					m_game->changeMusicVolume(m_game->getSavedMusicVolume());
					m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
				}
			}
		}
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "FORCEQUIT") {
			exit(1);
		}
	}
	else if (action.type() == "END") {
		if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
	}
}

void Scene_Floor::sAnimation() {
	auto& pState = m_player->getComponent<CState>();
	auto& currentAnim = m_player->getComponent<CAnimation>().animation;
	
	while (true) {
		if (pState.state == "Standing") {
			if (currentAnim.getName() != "GuyStand")
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyStand"), true);
			break;
		}
		if (pState.state == "WalkingUp") {
			if (currentAnim.getName() != "GuyWalkUp")
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyWalkUp"), true);
			break;
		}
		if (pState.state == "WalkingDown") {
			if (currentAnim.getName() != "GuyWalkDown")
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyWalkDown"), true);
			break;
		}
		if (pState.state == "WalkingRight") {
			if (currentAnim.getName() != "GuyWalkSide") {
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyWalkSide"), true);
			}
			// put next line inside if statement for side moon walk
			m_player->getComponent<CTransform>().scale.x = -1;
			break;
		}
		if (pState.state == "WalkingLeft") {
			if (currentAnim.getName() != "GuyWalkSide") {
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("GuyWalkSide"), true);
			}
			// put next line inside if statement for side moon walk
			m_player->getComponent<CTransform>().scale.x = 1;
			break;
		}
		break;
	}

	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CAnimation>()) {
			e->getComponent<CAnimation>().animation.update();
			if (!e->getComponent<CAnimation>().repeat) {
				if (e->getComponent<CAnimation>().animation.hasEnded()) {
					e->destroy();
				}
			}
		}
	}
}

void Scene_Floor::onEnd() {

	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), false);
}

void Scene_Floor::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Floor::sRender() {
	if (!m_paused) {
		m_game->window().clear(sf::Color(100, 100, 100));
	}
	else {
		m_game->window().clear(sf::Color(40, 40, 40));
	}

	sf::View view = m_game->window().getView();
	view.setCenter(m_currentRoom.x * 64 + m_game->window().getSize().x / 2.0f, m_currentRoom.y * 64 + m_game->window().getSize().y / 2.0f);
	m_game->window().setView(view);

	if (m_drawTextures) {
		for (auto e : m_entityManager.getEntities()) {
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>() && e->tag() != "Button") {
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}

			if (m_paused && e->tag() == "Button") {
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setPosition(m_game->window().getPosition().x + m_game->window().getSize().x / 2.0f, m_game->window().getPosition().y + m_game->window().getSize().y / 3.0f + e->getComponent<CId>().id * 300);
				if (e->getComponent<CId>().id == m_pauseIndex) {
					animation.getSprite().setColor(sf::Color::Red);
				}
				else {
					animation.getSprite().setColor(sf::Color::White);
				}
				m_game->window().draw(animation.getSprite());

				m_pauseText.setString(m_pauseStrings[e->getComponent<CId>().id]);
				m_pauseText.setFillColor(sf::Color::White);
				m_pauseText.setPosition(animation.getSprite().getPosition().x - m_pauseText.getLocalBounds().width / 2.0f, animation.getSprite().getPosition().y - 15);
				m_game->window().draw(m_pauseText);
			}
		}
	}

	if (m_drawCollision) {
		for (auto e : m_entityManager.getEntities()) {
			if (e->hasComponent<CBoundingBox>()) {
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_drawGrid) {
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
			drawLine(sf::Vector2f(x, 0), sf::Vector2f(x, height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y) {
			drawLine(sf::Vector2f(leftX, height() - y), sf::Vector2f(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}

	m_game->window().display();
}