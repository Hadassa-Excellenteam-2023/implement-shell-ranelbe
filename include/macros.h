#pragma once
#include <string>
#include <vector>

struct Command {
	std::vector<std::string> tokens;
	bool isBackground;
};

using commands = std::vector<Command>;