#include "Room.hpp"

	Room::Room() {}

	Room::Room(int maxNeighbors, int fakePosition)
		: r_maxNeighbors(maxNeighbors)
		, r_fakePosition(fakePosition)
		, r_remainingNeighbors(maxNeighbors) {
		r_rooms = std::vector<Room*>(4, nullptr);
	}

	Room* Room::getNeighborRoom(int direction) {
		return r_rooms[direction];
	}

	bool Room::remainsNeighbors() const {

		int count = r_maxNeighbors;

		if (r_rooms.at(0) != nullptr) {
			count--;
		}
		if (r_rooms.at(1) != nullptr) {
			count--;
		}
		if (r_rooms.at(2) != nullptr) {
			count--;
		}
		if (r_rooms.at(3) != nullptr) {
			count--;
		}
		if (count > 0)
			return true;
		return false;
	}

	void Room::setPlaced(bool placed) {
		r_placed = placed;
	}

	bool Room::isPlaced() const {
		return r_placed;
	}

	int Room::remainingNeighbors() const {
		return r_remainingNeighbors;
	}

	void Room::setNeighbor(int entry, Room* room) {
		r_rooms[entry] = room;
	}

	sf::Vector2i Room::getRoomPos() const {
		return r_pos;
	}

	int Room::getFakePos() const {
		return r_fakePosition;
	}

	int Room::getMaxNeighbors() const {
		return r_maxNeighbors;
	}

	sf::Vector2f Room::getRealPos() const
	{
		return r_realPos;
	}

	void Room::decrementNeighbors() {
		r_remainingNeighbors--;
	}

	void Room::setRoomPos(sf::Vector2i pos) {
		r_pos = pos;
	}

	void Room::setRealPos(unsigned int windowHeight) {
		r_realPos = { static_cast<float>(r_pos.x) * 64 * 30, static_cast<float>(windowHeight) - r_pos.y * 64 * 17 };
	}

	void Room::printInfo() const {
		std::cout << getFakePos() + 1 << "\tPosition : " << getRoomPos().x << ", " << getRoomPos().y << "\t" << "\tneighbors : "
			<< getMaxNeighbors() << std::endl;

		for (auto& e : r_rooms) {
			if (e != nullptr)
				std::cout << e->getFakePos() + 1 << " : " << e->getRoomPos().x << ", " << e->getRoomPos().y << std::endl;
		}
	}