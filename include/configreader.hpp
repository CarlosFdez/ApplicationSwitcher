#pragma once

#include <vector>
#include <fstream>
#include <string>
#include "switcher.hpp"

/*
Defines a structure used to define the behavior of the ApplicationSwitcher
for a single application.
*/
struct SwitcherEntry {
	std::string name;
	std::string hotkey;
	ApplicationFilter filter;
};

/* Reads a config file disk stored as json */
std::vector<SwitcherEntry> readConfig(const std::string& filepath);
