#pragma once

#include <string>
#include <vector>
#include <map>

#define PATH_MAX 4096

constexpr auto ERROR_UNKNOWN_DIR = "Unknown Directory";
constexpr auto ERROR_UNSPECIFIED_DIR = "cd: Directory not specified\n";
constexpr auto ERROR_CHANGE_DIR = "cd: Failed to change directory\n";
constexpr auto ERROR_COMMAND_NOT_FOUND = "Command not found\n";
constexpr auto ERROR_CREATE_CHILD = "Failed to create child process\n";
constexpr auto ERROR_EXECUTE_COMMAND = "Failed to execute command\n";
constexpr auto ERROR_CHECK_STATUS = "Error checking process status\n";


class Shell
{
public:
	void run();
	
private:
	std::string getCurrentDirectory() const;
	void changeDirectory(const std::vector<std::string>& tokens) const;
	std::vector<std::string> extractTokens(const std::string& input) const;
	void executeCommand(const std::vector<std::string>& tokens, bool isBackground);
	std::string findExecutablePath(const std::string& command) const;
	void executeChild(const std::string& executablePath, const std::vector<std::string>& tokens);
	void showJobs();

	std::map<pid_t, std::string> m_jobs;
};