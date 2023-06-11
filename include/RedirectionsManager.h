#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <fcntl.h>
#include <optional>


constexpr auto ERROR_MISSING_FILENAME = "Missing file name for redirection\n";
constexpr auto ERROR_STORE_FD = "Failed to store file descriptor\n";
constexpr auto ERROR_OPEN_FILE = "Failed to open the file";
constexpr auto ERROR_REDIRECT_FD = "Failed to redirect file descriptor\n";
constexpr auto ERROR_CLOSE_FD = "Failed to close file descriptor\n";
constexpr auto ERROR_RESTORE_FD = "Failed to restore original file descriptor\n";



class RedirectionsManager
{

public:
    static RedirectionsManager& instance() {
        static RedirectionsManager instance;
        return instance;
    }
	void handleRedirections(std::vector<std::string>& tokens);
    void reset();
private:
    RedirectionsManager() = default;
	void redirectIO(int stdFd, const std::string& file, int flags);
  
    std::unordered_map<std::string, std::pair<int, int>> _redirectionSymbols = {
        {">", {STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC}},
        {">>", {STDOUT_FILENO, O_WRONLY | O_CREAT | O_APPEND}},
        {"<", {STDIN_FILENO, O_RDONLY}}
    };

    std::optional<std::pair<int, int>> m_originalFd;
    
};