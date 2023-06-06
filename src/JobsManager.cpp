#include "JobsManager.h"
#include <iostream>
#include <sys/wait.h>


/*
* show the jobs that are currently running.
* display the PID, command and status of each job.
* kill jobs that have finished running.
*/
void JobsManager::showBackgroundJobs() const
{
    for (const auto& job : m_jobs) {
        std::cout << "PID: " << job.first << ", Command: "
            << job.second << ", Status: Running\n";
    }
}


/*
* clean up the jobs that have finished running.
*/
void JobsManager::cleanUpBackgroundJobs()
{
    for (auto it = m_jobs.cbegin(); it != m_jobs.cend();)
    {
        pid_t result = waitpid(it->first, nullptr, WNOHANG);
        if (result > 0) {
            m_jobs.erase(it++);
        }
        else {
            ++it;
        }
    }
}


/*
* add a job to the jobs list.
* @param pid - the process id of the job.
* @param command - the command of the job.
*/
void JobsManager::addJob(const pid_t& pid, const std::string& command)
{
    m_jobs[pid] = command;
}

