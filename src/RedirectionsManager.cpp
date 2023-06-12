#include "RedirectionsManager.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>


/*
* redirect the IO to the given file.
* @param stdFd - the std file descriptor to redirect.
* @param filename - the file to redirect to.
* @param flags - the flags for the open system call.
*/
void RedirectionsManager::redirectIO(const std::string& filename, int flags, int stdFd)
{
    // Store the original file descriptor before redirection
    int originalFd = dup(stdFd);
    if (originalFd == -1) {
        throw std::runtime_error(ERROR_STORE_FD);
    }

    // Open the file
    int fd = open(filename.c_str(), flags, 0666);
    if (fd == -1) {
        throw std::runtime_error(ERROR_OPEN_FILE);
    }

    // Redirect the file descriptor
    if (dup2(fd, stdFd) == -1) {
        close(fd);
        throw std::runtime_error(ERROR_REDIRECT_FD);
    } 
    if (close(fd) == -1) {
		throw std::runtime_error(ERROR_CLOSE_FD);
	}

    // store the original file descriptor
    m_originalFd = std::make_pair(originalFd, stdFd);
}


/*
* reset the IO to the original file descriptor.
*/
void RedirectionsManager::reset()
{
    //check if the pair is not empty
    if (m_originalFd) {
        if (dup2(m_originalFd->first, m_originalFd->second) == -1) {
            throw std::runtime_error(ERROR_RESTORE_FD);
        }
        if (close(m_originalFd->first) == -1) {
            throw std::runtime_error(ERROR_CLOSE_FD);
        }
	}
    m_originalFd.reset();
}