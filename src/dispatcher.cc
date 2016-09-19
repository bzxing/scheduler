
#include "dispatcher.hh"
#include "jobs.hh"
#include "workers.hh"

#include <vector>
#include <cassert>
#include <iostream>

namespace DISPATCHER
{

typedef JOBS::JOB_QUEUE JOB_QUEUE;
typedef JOB_QUEUE::ITER JOBQ_ITER;

namespace
{


JOBQ_ITER l_pick_best_job_to_execute()
{
	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();
	assert(job_q.begin() != job_q.end());
	WORKERS::WORKER_MGR::get_inst().get_eta(job_q.begin()->get());
	return job_q.begin();
}

// Send job to workers and dequeue it.
void l_dispatch(JOBQ_ITER jobq_iter)
{
	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();
	assert(jobq_iter != job_q.cend());
	const JOBS::JOB_ENTRY & job = jobq_iter->get();
	//std::cout << "Dispatching job " << job.to_string() << std::endl;
	job_q.erase(jobq_iter); // TODO
	WORKERS::WORKER_MGR::get_inst().submit_job(job);
}

} // End anonymous namespace

void dispatch_all()
{
	const auto & worker_mgr = WORKERS::WORKER_MGR::get_inst();
	std::cout << "Start dispatching jobs to workers...\n";
	JOBS::JOB_QUEUE & job_q = JOBS::JOB_QUEUE::get_inst();
	if (job_q.empty())
	{
		std::cerr << "No jobs to dispatch. Quitting...\n";
		exit(1);
	}
	while (!job_q.empty())
	{
		JOBQ_ITER best_job = l_pick_best_job_to_execute();
		l_dispatch(best_job);
	}
	std::cout << "Done dispatching! Here's the result: \n";
	std::cout << worker_mgr;
	assert(worker_mgr.execution_history_is_legal());
	std::cout << "Dispatching big success!\n";
}


} // End namespace DISPATCHER
