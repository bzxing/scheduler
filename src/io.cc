
#include "jobs.hh"
#include "workers.hh"
#include "dispatcher.hh"

#include <iostream>
#include <string>
#include <regex>
#include <cassert>



namespace IO
{


namespace
{

JOBS::JOB_ENTRY l_string_to_job_entry(const std::string & line)
{
	std::regex regex("(\\w+) (\\w+) (\\d+) (\\d+) (\\d+) (\\d+)");
	std::smatch match;
	std::regex_search(line, match, regex);

	assert(match.size() == 7);
	assert(match[1] == "job");

	JOBS::JOB_ENTRY job(match[2], std::stoul(match[6]), std::stoul(match[3]), std::stoul(match[5]), std::stoul(match[4]));
	return job;
}

WORKERS::WORKER l_string_to_worker_entry(const std::string & line)
{
	std::regex regex("(\\w+) (\\w+)");
	std::smatch match;
	std::regex_search(line, match, regex);

	assert(match.size() == 3);
	assert(match[1] == "worker");

	WORKERS::WORKER worker(match[2]);
	return worker;
}


}


void load_from_stdin()
{
	std::cout << "Start reading from stdin!" << std::endl;
	while (std::cin.good())
	{
		std::string line;
		std::getline(std::cin, line);
		if (line.empty()) { continue; }
		//std::cout << line << std::endl;
		std::regex regex("(^\\w+)");
		std::smatch match;
		std::regex_search(line, match, regex);

		assert(!match.empty());

		if (match[0] == "job")
		{
			JOBS::PARSED_JOBS::get_inst().add_job(l_string_to_job_entry(line));
		}
		else if (match[0] == "worker")
		{
			WORKERS::WORKER_MGR::get_inst().add_worker(l_string_to_worker_entry(line));
		}
		else
		{
			std::cerr << "Error: Unexpected line from input file:\n";
			std::cerr << line << std::endl;
			exit(1);
		}
	}

	//std::cout << "Done parsing! Here's the results:" << std::endl;
	//std::cout << JOBS::PARSED_JOBS::get_inst();

	if (JOBS::PARSED_JOBS::get_inst().empty())
	{
		std::cerr << "No jobs to do. Quitting...";
		exit(1);
	}
	if (WORKERS::WORKER_MGR::get_inst().empty())
	{
		std::cerr << "No workers found. Quitting...";
		exit(1);
	}
}

}

int main()
{
	IO::load_from_stdin();
	JOBS::JOB_QUEUE::load();
	DISPATCHER::dispatch_all();
	return 0;
}
