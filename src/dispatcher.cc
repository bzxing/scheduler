
#include "dispatcher.hh"
#include "jobs.hh"
#include "workers.hh"

#include <vector>
#include <cassert>
#include <iostream>
#include <limits>

namespace DISPATCHER
{

typedef JOBS::JOB_QUEUE JOB_QUEUE;
typedef JOB_QUEUE::ITER JOBQ_ITER;

namespace
{



JOBQ_ITER l_pick_best_job_to_execute()
{
	const size_t MAX_NUM_JOBS_TO_TRY = 5; // TODO: QoR Tuning

	WORKERS::WORKER_MGR & worker_mgr = WORKERS::WORKER_MGR::get_inst();

	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();

	JOBS::JOB_QUEUE::ITER job_iter = job_q.begin();
	assert(job_iter != job_q.end());
	size_t num_jobs_tried = 0;

	float smallest_cost_seen = std::numeric_limits<float>::max();
	JOBS::JOB_QUEUE::ITER best_job_iter;

	while (job_iter != job_q.end() && num_jobs_tried < MAX_NUM_JOBS_TO_TRY)
	{

		float eta = worker_mgr.get_projected_job_status(job_iter->get()).get_complete_time();
		float priority = job_iter->get().get_priority();
		float cost = eta / priority; // TODO: QoR Tuning
		if (cost < smallest_cost_seen)
		{
			smallest_cost_seen = cost;
			best_job_iter = job_iter;
		}
		std::cout << "Tested job " << job_iter->get().to_string()
			<< " ETA=" << eta << " Cost=" << cost << " Best Cost=" << smallest_cost_seen << std::endl;

		++job_iter;
		++num_jobs_tried;
	}

	return best_job_iter;
}

// Send job to workers and dequeue it.
void l_dispatch(JOBQ_ITER jobq_iter)
{
	JOB_QUEUE & job_q = JOB_QUEUE::get_inst();
	assert(jobq_iter != job_q.cend());
	JOBS::JOB_ENTRY & job = jobq_iter->get();
	std::cout << "Dispatching job " << job.to_string() << std::endl;
	auto & worker_mgr = WORKERS::WORKER_MGR::get_inst();
	worker_mgr.submit_job(job, job.get_modifiable_status());
	job_q.erase(jobq_iter);
	//std::cout << "Workers:\n" << worker_mgr;
	//std::cout << "Job Q:\n" << job_q;

}

} // End anonymous namespace

void dispatch_all()
{
	const auto & worker_mgr = WORKERS::WORKER_MGR::get_inst();
	JOBS::JOB_QUEUE & job_q = JOBS::JOB_QUEUE::get_inst();
	if (job_q.empty())
	{
		std::cerr << "No jobs to dispatch. Quitting...\n";
		exit(1);
	}
	std::cout << "Start dispatching jobs to workers...\n";
	while (!job_q.empty())
	{
		JOBQ_ITER best_job = l_pick_best_job_to_execute();
		l_dispatch(best_job);
	}
	std::cout << "Done dispatching! Here's the subtask history on each machine: \n";
	std::cout << worker_mgr;
	assert(worker_mgr.execution_history_is_legal());
	//TODO: Check all jobs are executed once and only once. Write some more checks.
	std::cout << "Here's the overall job status after dispatching all:\n";
	std::cout << JOBS::JOB_POOL::get_inst();
}


} // End namespace DISPATCHER
