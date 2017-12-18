#pragma once

#include <vector>
#include <fstream>
#include <string>
#include "switcher.hpp"

std::vector<SwitcherEntry> readConfig(const std::string& filepath);
