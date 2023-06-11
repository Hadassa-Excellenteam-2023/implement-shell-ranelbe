#pragma once

#include <string>
#include <vector>
#include <optional>

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
    void redirectIO(const std::string& filename, int flags, int stdFd);
    void reset();
private:
    RedirectionsManager() = default;
	
    std::optional<std::pair<int, int>> m_originalFd;  
};