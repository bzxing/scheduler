
#include "dispatcher.hh"
#include "jobs.hh"

#include <vector>
#include <cassert>
#include <iostream>

namespace DISPATCHER
{

typedef JOBS::JOB_QUEUE JOB_QUEUE;
typedef JOB_QUEUE::ITER JOB_ITER;

namespace
{


JOB_ITER l_pick_best_job_to_execute()
{
	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();
	assert(job_q.begin() != job_q.end());
	return job_q.begin();
}

void l_dispatch(JOB_ITER job_iter)
{
	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();
	assert(job_iter != job_q.cend());
	std::cout << "Dispatching job " << job_iter->to_string() << std::endl;
	job_q.erase(job_iter); // TODO
}

} // End anonymous namespace

void dispatch_all()
{
	std::cout << "Start dispatching jobs to workers...\n";
	JOBS::JOB_QUEUE & job_q = JOBS::JOB_QUEUE::get_inst();
	if (job_q.empty())
	{
		std::cerr << "No jobs to dispatch. Quitting...\n";
		exit(1);
	}
	while (!job_q.empty())
	{
		JOB_ITER best_job = l_pick_best_job_to_execute();
		l_dispatch(best_job);
	}
	std::cout << "Done dispatching!\n";
}


} // End namespace DISPATCHER
