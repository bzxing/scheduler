
#include "workers.hh"

#include <string>
#include <iostream>
#include <cassert>
#include <limits>
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

SUBTASK::SUBTASK(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time)
: m_job(job), m_start_time(start_time)
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

WORKER::SUBTASK_CITER WORKER::submit_subtask(const JOBS::JOB_ENTRY & job)
{
	auto iter_time_pair = find_earliest_subtask_insertion_slot_and_start_time(job, m_exec_hist);
	return m_exec_hist.emplace(iter_time_pair.first, job, iter_time_pair.second);
}

// Ruturn a copy of how the subtask would look like (start and complete time) if it were submitted,
// but don't really change the execution history
SUBTASK WORKER::try_submit_subtask(const JOBS::JOB_ENTRY & job) const
{
	auto iter_time_pair = find_earliest_subtask_insertion_slot_and_start_time(job, m_exec_hist);
	return SUBTASK(job, iter_time_pair.second);
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

std::ostream & operator<<(std::ostream & os, const WORKER & worker)
{
	os << "Worker " << worker.get_name() << " execution history: \n";
	for (const SUBTASK & hist_entry: worker.m_exec_hist)
	{
		os << "  " << hist_entry.to_string() << std::endl;
	}
	return os;
}

WORKER::WORKER(WORKER_NAME && name)
: m_name(std::move(name))
{

}

const WORKER_NAME & WORKER::get_name() const
{
	return m_name;
}

void WORKER_MGR::add_worker(WORKER && worker)
{
	std::cout << "Hello worker " << worker.get_name() << std::endl;
	m_workers.push_back(std::move(worker));
}

void WORKER_MGR::submit_job(const JOBS::JOB_ENTRY & job)
{
	assert(!empty());
	SUBMISSION_LIST submission_list;
	//std::cout << "Workers accepting job: " << job.to_string() << std::endl;
	JOBS::TIME start_time = std::numeric_limits<JOBS::TIME>::max();
	JOBS::TIME end_time = std::numeric_limits<JOBS::TIME>::min();
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
		WORKER::SUBTASK_CITER subtask_citer = best_worker_iter->submit_subtask(job);
		start_time = std::min(start_time, subtask_citer->get_start_time());
		end_time = std::max(end_time, subtask_citer->get_complete_time());
	}
	// Done submission. Now register job as submitted and mark start and end time.
	// job_status.set_as_submitted(start_time, end_time);
	#pragma message "TODO: Mark job final start & completion time"
}

JOBS::TIME WORKER_MGR::get_eta(const JOBS::JOB_ENTRY & job)
{
	assert(&job);
	#pragma message "TODO: get job ETA"
	return 0;
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
