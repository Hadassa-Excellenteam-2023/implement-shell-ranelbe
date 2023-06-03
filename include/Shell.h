#pragma once

#include <string>
#include <vector>
#include <map>

#define PATH_MAX 4096

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