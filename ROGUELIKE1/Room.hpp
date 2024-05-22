#pragma once

#include "Common.hpp"

class Room {

	bool					r_placed = false;

	int						r_fakePosition = 0;
	sf::Vector2i			r_pos = { 0, 0 };
	sf::Vector2f			r_realPos = { 0, 0 };
	
	int						r_maxNeighbors = 4;
	int						r_remainingNeighbors = 4;

	std::vector<Room*>		r_rooms;

public:

	Room();
	Room(int maxNeighbors, int fakePosition);
	Room* getNeighborRoom(int direction);
	void setPlaced(bool placed);
	bool isPlaced() const;
	bool remainsNeighbors() const;
	int remainingNeighbors() const;
	void setNeighbor(int entry, Room* room);

	void printInfo() const;

	void setRealPos(unsigned int windowHeight);
	sf::Vector2i getRoomPos() const;
	sf::Vector2f getRealPos() const;
	int getFakePos() const;
	int getMaxNeighbors() const;
	void setRoomPos(sf::Vector2i pos);

	void decrementNeighbors();
};