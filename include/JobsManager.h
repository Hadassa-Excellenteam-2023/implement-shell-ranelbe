#pragma once

#include <string>
#include <map>
#include <unistd.h>

class JobsManager
{
public:

	static JobsManager& instance() {
		static JobsManager instance;
		return instance;
	}

	void showBackgroundJobs() const;
	void cleanUpBackgroundJobs();
	void addJob(const pid_t& pid, const std::string& command);
	
private:
	JobsManager() = default;
	std::map<pid_t, std::string> m_jobs;

};