#pragma once

#include <vector>
#include <fstream>
#include <string>
#include "switcher.hpp"

/* Reads a config file disk stored as json */
std::vector<SwitcherEntry> readConfig(const std::string& filepath);
