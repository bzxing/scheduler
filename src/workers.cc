
#include "workers.hh"

#include <string>
#include <iostream>
#include <cassert>

namespace WORKERS
{

WORKER_MGR * WORKER_MGR::m_inst = nullptr;

namespace
{

JOBS::TIME l_get_job_completion_time(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time)
{
	assert(job.get_subtask_duration() > 0);
	return start_time + job.get_subtask_duration();
}

}

EXEC_HISTORY_ENTRY::EXEC_HISTORY_ENTRY(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time)
: m_job(job), m_start_time(start_time)
{
	assert(m_start_time >= m_job.get_earliest_start_time());
	assert(m_job.get_subtask_duration() > 0);
}

JOBS::TIME EXEC_HISTORY_ENTRY::get_start_time() const
{
	return m_start_time;
}

JOBS::TIME EXEC_HISTORY_ENTRY::get_complete_time() const
{
	return l_get_job_completion_time(m_job, m_start_time);
}

const JOBS::JOB_ENTRY & EXEC_HISTORY_ENTRY::get_job() const
{
	return m_job;
}

std::string EXEC_HISTORY_ENTRY::to_string() const
{
	std::string retval = m_job.to_string();
	retval += " ";
	retval += std::to_string(get_start_time());
	retval += " ";
	retval += std::to_string(get_complete_time());
	return retval;
}

WORKER_ENTRY::CITER WORKER_ENTRY::cbegin() const
{
	return m_exec_hist.cbegin();
}

WORKER_ENTRY::CITER WORKER_ENTRY::cend() const
{
	return m_exec_hist.cend();
}

WORKER_ENTRY::CITER WORKER_ENTRY::insert_job_at_earliest_possible_slot(const JOBS::JOB_ENTRY & job)
{
	auto iter = m_exec_hist.cbegin();
	JOBS::TIME prev_complete_time = 0;

	for (; iter != m_exec_hist.cend(); ++iter)
	{
		JOBS::TIME next_start_time = iter->get_start_time();
		assert(next_start_time >= prev_complete_time);
		JOBS::TIME idle_window = next_start_time - prev_complete_time;
		// Calculate the "would-be" start and end time for new job
		if (job.get_subtask_duration() <= idle_window)// can fit into the hole
		{
			// This is the right hole
			break;
		}
		prev_complete_time = iter->get_complete_time();
	}

	return m_exec_hist.emplace(iter, job, prev_complete_time);
}

bool WORKER_ENTRY::execution_history_is_legal() const
{
	JOBS::TIME prev_complete_time = 0;
	for (const auto & entry: m_exec_hist)
	{
		if (entry.get_start_time() < prev_complete_time)
		{
			return false;
		}
	}
	return true;
}

std::ostream & operator<<(std::ostream & os, const WORKER_ENTRY & worker)
{
	os << "Worker " << worker.get_name() << " execution history: \n";
	for (const EXEC_HISTORY_ENTRY & hist_entry: worker.m_exec_hist)
	{
		os << "  " << hist_entry.to_string() << std::endl;
	}
	return os;
}

WORKER_ENTRY::WORKER_ENTRY(WORKER_NAME && name)
: m_name(std::move(name))
{

}

const WORKER_NAME & WORKER_ENTRY::get_name() const
{
	return m_name;
}

void WORKER_MGR::add_worker(WORKER_ENTRY && worker)
{
	std::cout << "Hello worker " << worker.get_name() << std::endl;
	m_workers.push_back(std::move(worker));
}

void WORKER_MGR::submit_job(const JOBS::JOB_ENTRY & job)
{
	assert(!empty());
	//std::cout << "Workers accepting job: " << job.to_string() << std::endl;
	for (size_t i_subtask = 0; i_subtask < job.get_num_subtasks(); ++i_subtask)
	{
		// TODO: submit to worker 0 for now.
		begin()->insert_job_at_earliest_possible_slot(job);
	}
}

JOBS::TIME WORKER_MGR::get_eta(const JOBS::JOB_ENTRY & job)
{
	//std::cout << "Querying Job ETA: " << job.to_string() << std::endl;
	// TODO
	return 0;
}

WORKER_MGR::ITER WORKER_MGR::begin()
{
	return m_workers.begin();
}

WORKER_MGR::ITER WORKER_MGR::end()
{
	return m_workers.end();
}

WORKER_MGR::CITER WORKER_MGR::cbegin() const
{
	return m_workers.cbegin();
}

WORKER_MGR::CITER WORKER_MGR::cend() const
{
	return m_workers.cend();
}

bool WORKER_MGR::execution_history_is_legal() const
{
	std::cout << "Checking worker execution history legality...\n";
	for (const WORKER_ENTRY & worker: m_workers)
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
	for (const WORKER_ENTRY & worker: worker_mgr.m_workers)
	{
		os << worker;
	}
	return os;
}


}
