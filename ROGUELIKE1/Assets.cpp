#include "Assets.hpp"
#include "Common.hpp"
#include <cassert>

Assets::Assets() = default;

void Assets::loadFromFile(const std::string& path) {
	std::ifstream file(path);
	std::string str;

	while (file.good() && str != " ") {
		file >> str;
		if (str == "Texture") {
			std::string name, path;
			file >> name >> path;
			addTexture(name, path);
		}
		else if (str == "Animation") {
			std::string name, texture;
			size_t frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Font") {
			std::string name, path;
			file >> name >> path;
			addFont(name, path);
		}
		else {
			std::cerr << "Unknown Asset Type : " << str << std::endl;
		}
	}
}

void Assets::addTexture(const std::string& textureName, const std::string& path, bool smooth) {
	m_textureMap[textureName] = sf::Texture();

	if (!m_textureMap[textureName].loadFromFile(path)) {
		std::cerr << "Could not load texture from file : " << path << std::endl;
		m_textureMap.erase(textureName);
	}
	else {
		m_textureMap[textureName].setSmooth(smooth);
		std::cout << "Loaded Texture : " << path << std::endl;
	}
}

void Assets::addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed) {
	m_animationMap[animationName] = Animation(animationName, getTexture(textureName), frameCount, speed);
}

void Assets::addFont(const std::string& fontName, const std::string& path) {
	m_fontMap[fontName] = sf::Font();
	if (!m_fontMap[fontName].loadFromFile(path)) {
		std::cerr << "Could not load font : " << fontName << std::endl;
		m_fontMap.erase(fontName);
	}
	else {
		std::cout << "Loaded font : " << path << std::endl;
	}
}

const sf::Texture& Assets::getTexture(const std::string& textureName) const {
	assert(m_textureMap.find(textureName) != m_textureMap.end());
	return m_textureMap.at(textureName);
}

const Animation& Assets::getAnimation(const std::string& animationName) const {
	assert(m_animationMap.find(animationName) != m_animationMap.end());
	return m_animationMap.at(animationName);
}

const sf::Font& Assets::getFont(const std::string& fontName) const {
	assert(m_fontMap.find(fontName) != m_fontMap.end());
	return m_fontMap.at(fontName);
}

const sf::Vector2f Assets::getSpriteSize(std::string name) const {
	return static_cast<sf::Vector2f>(m_animationMap.at(name).getSize());
}