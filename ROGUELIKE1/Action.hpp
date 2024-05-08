#pragma once

#include "Common.hpp"

class Action {

	std::string m_name = "default";
	std::string m_type = "none";

public:
	Action();
	Action(const std::string& name, const std::string& type);

	const std::string& name() const;
	const std::string& type() const;
};