#include "Scene_Floor.hpp"
#include "Common.hpp"
#include "Physics.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "Action.hpp"
#include "Components.hpp"
#include "Scene_Menu.hpp"
#include "VectorHelp.hpp"
#include <queue>

Scene_Floor::Scene_Floor(GameEngine* gameEngine, int roomsSize)
	: Scene(gameEngine)
	, m_roomsSize(roomsSize) {
	init(generateFloor(m_game->getCurrentFloor()));
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

	m_currentRoom = m_rooms[0];

	m_fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
	m_fadeRect.setSize(static_cast<sf::Vector2f>(m_game->window().getSize()));
	m_fadeRect.setPosition(m_currentRoom->getRealPos().x, m_currentRoom->getRealPos().y - m_game->window().getSize().y);

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
				e->addComponent<CTransform>(gridToMidPixel(64 * x, static_cast<int>(m_game->window().getSize().y) - 64 * y, e));
				e->getComponent<CTransform>().prevPos = e->getComponent<CTransform>().pos;
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
				>> m_playerConfig.MAXSPEED >> m_playerConfig.WEAPON;
		}
		else if (str == "Enemy") {
			file >> m_bugConfig.X >> m_bugConfig.Y >> m_bugConfig.CX >> m_bugConfig.CY >> m_bugConfig.SPEED
				>> m_bugConfig.MAXSPEED >> m_bugConfig.HP >> m_bugConfig.ATTACK;
		}
	}

	spawnPlayer();
	spawnEnemy();
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

void Scene_Floor::spawnEnemy() {
	auto e = m_entityManager.addEntity("Enemy");
	e->addComponent<CAnimation>(m_game->assets().getAnimation("BugWalk"), true);
	e->addComponent<CState>("Moving");
	e->addComponent<CTransform>(gridToMidPixel(64 * m_bugConfig.X, m_game->window().getSize().y - 64 * m_bugConfig.Y, e));
	e->getComponent<CTransform>().scale.x = 64 / e->getComponent<CAnimation>().animation.getSize().x;
	e->getComponent<CTransform>().scale.y = 64 / e->getComponent<CAnimation>().animation.getSize().y;
	e->addComponent<CBoundingBox>(sf::Vector2f(m_bugConfig.CX, m_bugConfig.CY));
}

void Scene_Floor::update() {
	m_entityManager.update();

	if (!m_paused) {
		if (!m_roomSwitch) {
			sMovement();
			sLifespan();
			sCollision();
			m_currentFrame++;
		}
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

	//update speed for a side if that side's appropriate key is held, otherwise if player still moving, reduce velocity by speed
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

	// limit max speed for movements
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
	auto& pTransform = m_player->getComponent<CTransform>();

	if (pTransform.pos.x <= m_currentRoom->getRealPos().x) {
		if (m_currentRoom->getNeighborRoom(2) != nullptr) {
			m_roomExit = 2;
			startFade();
		}
	} 
	else if (pTransform.pos.x >= m_currentRoom->getRealPos().x + m_game->window().getSize().x) {
		if (m_currentRoom->getNeighborRoom(3) != nullptr) {
			m_roomExit = 3;
			startFade();
		}
	} 
	else if (pTransform.pos.y <= m_currentRoom->getRealPos().y - m_game->window().getSize().y) {
		if (m_currentRoom->getNeighborRoom(0) != nullptr) {
			m_roomExit = 0;
			startFade();
		}
	}
	else if (pTransform.pos.y >= m_currentRoom->getRealPos().y) {
		if (m_currentRoom->getNeighborRoom(1) != nullptr) {
			m_roomExit = 1;
			startFade();
		}
	}

	sf::Vector2f overlap;
	sf::Vector2f prevOverlap;
	for (auto tile : m_entityManager.getEntities()) {
		if (tile->tag() == "Tile") {
			auto& eTransform = tile->getComponent<CTransform>();
			overlap = Physics::GetOverlap(m_player, tile);
			prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

			if (overlap.x > 0 && overlap.y > 0) {
				if (prevOverlap.x > 0) {
					if (pTransform.pos.y < eTransform.pos.y) {
						pTransform.pos.y -= overlap.y;
					}
					else {
						pTransform.pos.y += overlap.y;
					}
				}
				else if (prevOverlap.y > 0) {
					if (pTransform.pos.x < eTransform.pos.x) {
						pTransform.pos.x -= overlap.x;
						
					}
					else {
						pTransform.pos.x += overlap.x;
					}
				}
			}
		}
	}
}

void Scene_Floor::sMoveEntity(sf::Vector2f roomPos, int entry, ptr<Entity> e) {

	auto& pTransform = e->getComponent<CTransform>();

	switch (entry) {
	case 0:
		pTransform.pos.x = roomPos.x + m_game->window().getSize().x / 2.0f;
		pTransform.pos.y = roomPos.y - m_game->window().getSize().y + 128;
		break;
	case 1:
		pTransform.pos.x = roomPos.x + m_game->window().getSize().x / 2.0f;
		pTransform.pos.y = roomPos.y - 128;
		break;
	case 2:
		pTransform.pos.x = roomPos.x + 128;
		pTransform.pos.y = roomPos.y - m_game->window().getSize().y / 2.0f;
		break;
	case 3:
		pTransform.pos.x = roomPos.x + m_game->window().getSize().x - 128;
		pTransform.pos.y = roomPos.y - m_game->window().getSize().y / 2.0f;
		break;
	default:
		break;
	}

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
					sf::View view = m_game->window().getView();
					view.setCenter(m_game->window().getSize().x / 2.0f,m_game->window().getSize().y / 2.0f);
					m_game->window().setView(view);
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



	//sf::View view(sf::FloatRect(-1920 * 4, -1080 * 4, 1920 * 4, 1080 * 4));
	sf::View view = m_game->window().getView();
	view.setCenter(m_currentRoom->getRoomPos().x * 30 *  64 + m_game->window().getSize().x / 2.0f, m_currentRoom->getRoomPos().y * 17 * -64 + m_game->window().getSize().y / 2.0f);
	m_game->window().setView(view);
	
	if (m_drawTextures) {
		for (auto e : m_entityManager.getEntities()) {
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>() && e->tag() != "Button" && !m_paused) {

				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());


				m_pauseText.setCharacterSize(50);
				sf::Color tempColor = { 255, 255, 255, 120};
				m_pauseText.setFillColor(tempColor);
				m_pauseText.setString(std::to_string(m_currentRoom->getRoomPos().x) + ", " + std::to_string(m_currentRoom->getRoomPos().y));

				m_pauseText.setPosition(m_game->window().getView().getCenter());
				m_game->window().draw(m_pauseText);
			}

			// render things when game is paused
			if (m_paused && e->tag() == "Button") {
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setPosition(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y - 200 + e->getComponent<CId>().id * 300);
				if (e->getComponent<CId>().id == m_pauseIndex) {
					animation.getSprite().setColor(sf::Color::Red);
				}
				else {
					animation.getSprite().setColor(sf::Color::White);
				}
				m_game->window().draw(animation.getSprite());

				m_pauseText.setCharacterSize(25);
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

	updateFade();
	m_game->window().display();
}


std::string Scene_Floor::generateFloor(int floorNum) {
	std::string path = "levels/floor" + std::to_string(floorNum) + ".txt";
	std::ofstream file(path);

	if (!file.is_open()) {
		std::cerr << "Error opening file : " + path << std::endl;
		exit(1);
	}

	srand(time(nullptr));
	std::vector<Room*> rooms;

	for (int i = 0; i < m_roomsSize; i++) {

		Room* room;

		if (i != 0)
		{
			int maxNeighbors;
			if (i % 2 == 0) {
				maxNeighbors = (rand() % 2) + 3;

			}
			else {
				maxNeighbors = (rand() % 4) + 1;
			}
			room = new Room(maxNeighbors, i);
		}
		else {
			room = new Room(4, 0);
			room->setPlaced(true);
			room->setRealPos(m_game->window().getSize().y);
			rooms.push_back(room);

		}
		m_rooms.push_back(room);
	}

	// rooms.size = 1 at start
	for (int j = 1; j < m_roomsSize; j++) {
		auto e = m_rooms.at(j);
		
		while (true) {
			auto randomRoom = rooms.at(rand() % rooms.size());
			if (randomRoom->remainingNeighbors() <= 0) {
				break;
			}
			int randomDirection = rand() % 4;

			sf::Vector2i tempVec = randomRoom->getRoomPos();

			if (randomDirection == 0) {
				tempVec.y += 1;
			}
			else if (randomDirection == 1) {
				tempVec.y -= 1;
			} else if (randomDirection == 2) {
				tempVec.x -= 1;
			}
			else {
				tempVec.x += 1;
			}
			//auto desiredSpot = randomRoom.getNeighborRoom(randomDirection);
			if (simulateRoomPlacement(tempVec)) {

				e->setPlaced(true);
				e->setRoomPos(tempVec);
				e->setRealPos(m_game->window().getSize().y);

				randomRoom->setNeighbor(randomDirection, e);
				e->setNeighbor(getOppositeSide(randomDirection), randomRoom);
				randomRoom->decrementNeighbors();
				e->decrementNeighbors();
				rooms.push_back(e);
				break;
			}
		}
		int index = 0;
		for (auto& e : rooms) {
			if (!e->remainsNeighbors()) {
				erase_at(rooms, index);
			}
			index++;
		}
	}
	
	for (auto& room : m_rooms) {
		if (room->isPlaced()) {
			sf::Vector2i roomPos = room->getRoomPos();

			int midX = roomPos.x * 30 + 15;
			int midY = roomPos.y * 17 + 8;

			// Write top border ** removed + 1 to midX since horizontal borders are even and verticals are odd
			for (int x = roomPos.x * 30; x < (roomPos.x + 1) * 30; x++) {
				if (!(room->getNeighborRoom(0) != nullptr && x >= midX - 1 && x <= midX))
					file << "Tile\tGreyBrick\t" << x << "\t" << roomPos.y * 17 + 16 << std::endl;
			}
			// Write left border
			for (int y = roomPos.y * 17; y < (roomPos.y + 1) * 17; y++) {
				if (!(room->getNeighborRoom(2) != nullptr && y >= midY - 1 && y <= midY + 1))
					file << "Tile\tGreyBrick\t" << roomPos.x * 30 << "\t" << y << std::endl;
			}
			// Write bottom border
			for (int x = roomPos.x * 30; x < (roomPos.x + 1) * 30; x++) {
				if (!(room->getNeighborRoom(1) != nullptr && x >= midX - 1 && x <= midX))
					file << "Tile\tGreyBrick\t" << x << "\t" << roomPos.y * 17 << std::endl;
			}
			// Write right border
			for (int y = roomPos.y * 17; y < (roomPos.y + 1) * 17; y++) {
				if (!(room->getNeighborRoom(3) != nullptr && y >= midY - 1 && y <= midY + 1))
					file << "Tile\tGreyBrick\t" << (roomPos.x + 1) * 30 - 1 << "\t" << y << std::endl;
			}
		}
	}

	for (int k = 0; k < 2; k++) {
		file << "Button\tPauseButton\t" << k << std::endl;
	}

	file << "Player\t" << (m_game->window().getSize().x / 64.0f) / 2.0f << "\t" << (m_game->window().getSize().y / 64.0f) / 2.0f << "\t"
		<< "32\t48\t0.5\t7" << std::endl;

	file << "Enemy\t" << (m_game->window().getSize().x / 64.0f) / 2.0f << "\t" << (m_game->window().getSize().y / 64.0f) / 2.0f << "\t"
		<< "20\t20\t1\t4\t50\tBite" << std::endl;

	file.close();

	std::cout << "\n" << std::endl;

	for (int i = 0; i < m_rooms.size(); i++) {
		m_rooms[i]->printInfo();
	}

	return path;
}

/*
sf::Vector2i Scene_Floor::generateMultiples(int roomsSize) {
	for (int i = 5; i > 1; i--) {
		if (roomsSize % i == 0)
			return { i, roomsSize / i };
	}
}
*/

// try to setup roomToPlace at placement coordinates, if already more than one room there, impossible to place
bool Scene_Floor::simulateRoomPlacement(sf::Vector2i placement) {
	int overlaid = 0;
	for (auto& e : m_rooms) {
		if (e->isPlaced()) {
			if (e->getRoomPos() == placement) {
				overlaid++;
			}
		}
		if (overlaid > 0) {
			return false;
		}
	}
	return true;
}

int Scene_Floor::getOppositeSide(int previousEntry) {
	if (previousEntry == 0) {
		return 1;
	}
	else if (previousEntry == 1) {
		return 0;
	}
	else if (previousEntry == 2) {
		return 3;
	}
	else {
		return 2;
	}
}

void Scene_Floor::cleanRooms() {
	for (auto room : m_rooms) {
		delete room;
	}

	m_rooms.clear();
}

void Scene_Floor::startFade() {
	m_player->getComponent<CTransform>().velocity = { 0, 0 };
	m_roomSwitch = true;
	m_fadeState = FadeIn;
	m_fadeClock.restart();
}

void Scene_Floor::updateFade() {
	if (m_fadeState != None) {
		float elapsed = m_fadeClock.getElapsedTime().asSeconds();
		float alpha = 0;

		if (m_fadeState == FadeIn) {
			alpha = 255 * (elapsed / 0.7f);
			if (elapsed >= 0.7f) {
				changeRoom(m_roomExit);
				alpha = 255;
				m_fadeState = FadeOut;
				m_fadeClock.restart();
			}
		}
		else if (m_fadeState == FadeOut) {
			alpha = 255 * (1 - (elapsed / 0.7f));
			if (elapsed >= 0.7f) {
				alpha = 0;
				m_fadeState = None;
				m_roomSwitch = false;
			}
		}

		m_fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
	}
	
	m_game->window().draw(m_fadeRect);
}

void Scene_Floor::changeRoom(int entry) {
	m_currentRoom = m_currentRoom->getNeighborRoom(entry);
	sMoveEntity(m_currentRoom->getRealPos(), getOppositeSide(entry), m_player);
	m_fadeRect.setPosition(m_currentRoom->getRealPos().x, m_currentRoom->getRealPos().y - m_game->window().getSize().y);
	sf::View view = m_game->window().getView();
	view.setCenter(m_currentRoom->getRoomPos().x * 30 * 64 + m_game->window().getSize().x / 2.0f, m_currentRoom->getRoomPos().y * 17 * -64 + m_game->window().getSize().y / 2.0f);
	m_game->window().setView(view);
}