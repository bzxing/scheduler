
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

// TODO: index should be separated from this
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

// TODO: index should be separated from this
WORKERS::WORKER l_string_to_worker_entry(const std::string & line, WORKERS::WORKER::WORKER_IDX idx)
{
	std::regex regex("(\\w+) (\\w+)");
	std::smatch match;
	std::regex_search(line, match, regex);

	assert(match.size() == 3);
	assert(match[1] == "worker");

	WORKERS::WORKER worker(match[2], idx);
	return worker;
}


}


void load_from_stdin()
{
	std::cout << "Start reading from stdin!" << std::endl;
	JOBS::JOB_POOL & job_pool = JOBS::JOB_POOL::get_inst();

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
			job_pool.add_job(l_string_to_job_entry(line));
		}
		else if (match[0] == "worker")
		{
			WORKERS::WORKER_MGR & worker_mgr = WORKERS::WORKER_MGR::get_inst();
			WORKERS::WORKER::WORKER_IDX new_idx = worker_mgr.size();
			worker_mgr.add_worker(l_string_to_worker_entry(line, new_idx));
		}
		else
		{
			std::cerr << "Error: Unexpected line from input file:\n";
			std::cerr << line << std::endl;
			exit(1);
		}
	}

	job_pool.sort_and_create_index();

	//std::cout << "Done parsing! Here's the results:" << std::endl;
	//std::cout << JOBS::JOB_POOL::get_inst();

	if (JOBS::JOB_POOL::get_inst().empty())
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
	JOBS::COST_CALC::get_total_cost();
	return 0;
}
