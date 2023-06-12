#pragma once

#include "macros.h"
#include <string>
#include <vector>

constexpr auto ERROR_COMMAND_NOT_FOUND = "Command not found\n";
constexpr auto ERROR_CREATE_CHILD = "Failed to create child process\n";
constexpr auto ERROR_EXECUTE_COMMAND = "Failed to execute command\n";


class Shell
{
public:
	void run();
	
private:
	void execute(const commands& commands);
	void executeSingleCommand(const Command& command);
	void executePipedCommand(const commands& commands);
	void executeChild(const std::string& executablePath, 
		const std::vector<std::string>& tokens);

};