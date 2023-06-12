#pragma once
#include "macros.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <fcntl.h>

constexpr auto ERROR_MISSING_FILENAME = "Missing file name for redirection\n";


class InputParser
{
public:
	static InputParser& instance() {
		static InputParser instance;
		return instance;
	}
	commands tokenize(const std::string& input);

private:
	InputParser() = default;

	std::unordered_map<std::string, std::pair<int, int>> m_redirectionSymbols = {
		{">", {STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC}},
		{">>", {STDOUT_FILENO, O_WRONLY | O_CREAT | O_APPEND}},
		{"<", {STDIN_FILENO, O_RDONLY}}
	};
};