#pragma once

#include <string>

#define PATH_MAX 4096
constexpr auto ERROR_UNKNOWN_DIR = "Unknown Directory";
constexpr auto ERROR_UNSPECIFIED_DIR = "cd: Directory not specified\n";
constexpr auto ERROR_CHANGE_DIR = "cd: Failed to change directory\n";

class DirectoryManager
{
public:
	static std::string getCurrentDirectory();
	static void changeDirectory(const std::string& newPath);
	static std::string findExecutablePath(const std::string& command);

private:

};