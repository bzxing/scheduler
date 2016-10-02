
#include "workers.hh"

#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>

namespace WORKERS
{

WORKER_MGR * WORKER_MGR::m_inst = nullptr;

namespace
{

typedef std::vector<WORKER::SUBTASK_CITER> SUBMISSION_LIST;


JOBS::TIME l_get_job_completion_time(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time)
{
	assert(job.get_subtask_duration() > 0);
	return start_time + job.get_subtask_duration();
}

std::pair<WORKER::SUBTASK_CITER, JOBS::TIME>
find_earliest_subtask_insertion_slot_and_start_time(
	const JOBS::JOB_ENTRY & job, const WORKER::SUBTASK_CONTAINER & exec_hist)
{
	JOBS::TIME prev_complete_time = 0;
	const JOBS::TIME earliest_start = job.get_earliest_start_time();

	// Find the right hole of right size where the job should be inserted.
	for (auto iter = exec_hist.cbegin(); iter != exec_hist.cend(); ++iter)
	{
		assert(iter->get_start_time() >= prev_complete_time);

		JOBS::TIME hole_end = iter->get_start_time();
		JOBS::TIME clamped_hole_start = std::max(prev_complete_time, earliest_start);

		prev_complete_time = iter->get_complete_time();

		if (clamped_hole_start > hole_end)
		{
			// Clamped by earliest_start that's after the hole
			continue;
		}

		JOBS::TIME hole_size = hole_end - clamped_hole_start;

		// Calculate the "would-be" start and end time for new job
		if (job.get_subtask_duration() <= hole_size)// can fit into the hole
		{
			// This is the right hole
			return std::make_pair(iter, clamped_hole_start);
		}
	}

	// No hole works. Put it at the end of list.
	return std::make_pair(exec_hist.cend(), std::max(earliest_start, prev_complete_time));
}


} // End anonymous namespace

SUBTASK::SUBTASK(const JOBS::JOB_ENTRY & job, const WORKER & worker, JOBS::TIME start_time)
: m_job(job), m_worker(worker), m_start_time(start_time)
{
	assert(m_start_time >= m_job.get_earliest_start_time());
	assert(m_job.get_subtask_duration() > 0);
}

JOBS::TIME SUBTASK::get_start_time() const
{
	return m_start_time;
}

JOBS::TIME SUBTASK::get_complete_time() const
{
	return l_get_job_completion_time(m_job, m_start_time);
}

const JOBS::JOB_ENTRY & SUBTASK::get_job() const
{
	return m_job;
}

std::string SUBTASK::to_string() const
{
	std::string retval = m_job.to_string();
	retval += ": ";
	retval += std::to_string(get_start_time());
	retval += " ";
	retval += std::to_string(get_complete_time());
	return retval;
}

WORKER::SUBTASK_CITER WORKER::cbegin() const
{
	return m_exec_hist.cbegin();
}

WORKER::SUBTASK_CITER WORKER::cend() const
{
	return m_exec_hist.cend();
}

WORKER::SUBTASK_ITER WORKER::submit_subtask(const JOBS::JOB_ENTRY & job)
{
	auto iter_time_pair = find_earliest_subtask_insertion_slot_and_start_time(job, m_exec_hist);
	return m_exec_hist.emplace(iter_time_pair.first, job, *this, iter_time_pair.second);
}

// Ruturn a copy of how the subtask would look like (start and complete time) if it were submitted,
// but don't really change the execution history
SUBTASK WORKER::try_submit_subtask(const JOBS::JOB_ENTRY & job) const
{
	auto iter_time_pair = find_earliest_subtask_insertion_slot_and_start_time(job, m_exec_hist);
	return SUBTASK(job, *this, iter_time_pair.second);
}

bool WORKER::execution_history_is_legal() const
{
	JOBS::TIME prev_complete_time = 0;
	for (const auto & entry: m_exec_hist)
	{
		if (entry.get_start_time() < prev_complete_time ||
			entry.get_start_time() < entry.get_job().get_earliest_start_time())
		{
			return false;
		}
	}
	return true;
}

WORKER::WORKER_IDX WORKER::get_index() const
{
	return m_idx;
}

std::ostream & operator<<(std::ostream & os, const WORKER & worker)
{
	os << "Worker #" << worker.m_idx << " " << worker.get_name() << " execution history: \n";
	for (const SUBTASK & hist_entry: worker.m_exec_hist)
	{
		os << "  " << hist_entry.to_string() << std::endl;
	}
	return os;
}

WORKER::WORKER(WORKER_NAME && name, WORKER_IDX idx)
: m_name(std::move(name)), m_idx(idx)
{

}

const WORKER_NAME & WORKER::get_name() const
{
	return m_name;
}

void WORKER::remove_subtask(SUBTASK_ITER subtask_iter)
{
	m_exec_hist.erase(subtask_iter);
}

void WORKER_MGR::add_worker(WORKER && worker)
{
	std::cout << "Hello worker #" << worker.get_index() << " " << worker.get_name() << std::endl;
	m_workers.push_back(std::move(worker));
}



// This is the actual submission algorithm that schedules subtasks across all machines.
// job_status will be updated to reflect the start & end time for all subtasks, no matter whether
// revert_after_trying is on or off.

void WORKER_MGR::try_submit_job(const JOBS::JOB_ENTRY & job, JOBS::JOB_STATUS & job_status, bool revert_after_trying)
{
	bool debug = false;

	assert(!empty());
	assert(job_status.is_clean());
	assert(job_status.get_parent() == job.get_index());
	job_status.reset();

	// If revert_after_trying == true, this vector is used to memorize submissions, and revert them
	// in the end. Useful when just want to check out the ETA of a job without submitting anything.
	std::vector<std::pair<WORKER_ITER, WORKER::SUBTASK_ITER>> submitted_subtasks;

	for (size_t i_subtask = 0; i_subtask < job.get_num_subtasks(); ++i_subtask)
	{
		// Pick worker with best completion time
		WORKER_ITER best_worker_iter = std::min_element(
			m_workers.begin(), m_workers.end(),
			[&job](const WORKER & lhs, const WORKER & rhs)
			{
				return bool(
					lhs.try_submit_subtask(job).get_complete_time() <
					rhs.try_submit_subtask(job).get_complete_time());
			}
		);

		// Submit it
		WORKER::SUBTASK_ITER subtask_iter = best_worker_iter->submit_subtask(job);
		job_status.add_subtask(*subtask_iter);

		if (revert_after_trying)
		{
			submitted_subtasks.push_back(std::make_pair(best_worker_iter, subtask_iter));
		}
	}

	if (revert_after_trying)
	{
		for (const auto & worker_subtask_iter_pair: submitted_subtasks)
		{
			worker_subtask_iter_pair.first->remove_subtask(worker_subtask_iter_pair.second);
		}
	}
	if (debug)
	{
		std::cout << job.to_string() << std::endl;
		std::cout << job_status.to_string() << std::endl;
	}
	assert(job_status.submitted());

	// TODO: Compress start time when possible. QoR measurement
}

void WORKER_MGR::submit_job(const JOBS::JOB_ENTRY & job, JOBS::JOB_STATUS & job_status)
{
	try_submit_job(job, job_status, false);
}


JOBS::JOB_STATUS WORKER_MGR::get_projected_job_status(const JOBS::JOB_ENTRY & job)
{
	JOBS::JOB_STATUS projected_status;
	projected_status.set_parent(job.get_index());
	try_submit_job(job, projected_status, true);
	return projected_status;
}

WORKER_MGR::WORKER_ITER WORKER_MGR::begin()
{
	return m_workers.begin();
}

WORKER_MGR::WORKER_ITER WORKER_MGR::end()
{
	return m_workers.end();
}

WORKER_MGR::WORKER_CITER WORKER_MGR::cbegin() const
{
	return m_workers.cbegin();
}

WORKER_MGR::WORKER_CITER WORKER_MGR::cend() const
{
	return m_workers.cend();
}

bool WORKER_MGR::execution_history_is_legal() const
{
	std::cout << "Checking worker execution history legality...\n";
	for (const WORKER & worker: m_workers)
	{
		if (worker.execution_history_is_legal() == false)
		{
			std::cout << "Found illegal worker: " << worker.get_name() << std::endl;
			std::cout << worker;
			return false;
		}
	}
	std::cout << "All legal!\n";
	return true;
}

size_t WORKER_MGR::size() const
{
	return m_workers.size();
}

bool WORKER_MGR::empty() const
{
	return m_workers.empty();
}

WORKER_MGR & WORKER_MGR::get_inst()
{
	if (m_inst == nullptr)
	{
		m_inst = new WORKER_MGR;
	}
	return *m_inst;
}

std::ostream & operator<<(std::ostream & os, const WORKER_MGR & worker_mgr)
{
	for (const WORKER & worker: worker_mgr.m_workers)
	{
		os << worker;
	}
	return os;
}


}
