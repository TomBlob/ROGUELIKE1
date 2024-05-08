#include "EntityManager.hpp"

EntityManager::EntityManager()
{

}

void EntityManager::update() {

	for (auto e : m_entitiesToAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	m_entitiesToAdd.clear();

	removeDeadEntities(m_entities);

	for (auto& [tag, entityVec] : m_entityMap) {
		removeDeadEntities(entityVec);
	}

}

void EntityManager::removeDeadEntities(EntityVector& vec) {

	auto iter = vec.begin();

	while (iter != vec.end()) {
		if (!(*iter)->isActive()) {
			iter = vec.erase(iter);
		}
		else {
			iter++;
		}
	}

}

ptr<Entity> EntityManager::addEntity(const std::string& tag) {

	auto entity = ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVector& EntityManager::getEntities() {
	return m_entities;
}

const EntityVector& EntityManager::getEntities(const std::string& tag) {

	return m_entityMap[tag];
}