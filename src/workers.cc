
#include "workers.hh"

#include <string>
#include <iostream>
#include <cassert>

namespace WORKERS
{

WORKER_MGR * WORKER_MGR::m_inst = nullptr;

namespace
{

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
	assert(m_job.get_subtask_duration() > 0);
	return m_start_time + m_job.get_subtask_duration() - 1;
}

const JOBS::JOB_ENTRY & EXEC_HISTORY_ENTRY::get_job() const
{
	return m_job;
}

WORKER_ENTRY::CITER WORKER_ENTRY::cbegin() const
{
	return m_exec_hist.cbegin();
}

WORKER_ENTRY::CITER WORKER_ENTRY::cend() const
{
	return m_exec_hist.cend();
}

void WORKER_ENTRY::append_job(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time)
{
	m_exec_hist.emplace_back(job, start_time);
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
	std::cout << "Workers accepting job: " << job.to_string() << std::endl;
}

JOBS::TIME WORKER_MGR::get_eta(const JOBS::JOB_ENTRY & job)
{
	std::cout << "Querying Job ETA: " << job.to_string() << std::endl;
	return 0;
}

WORKER_MGR::CITER WORKER_MGR::cbegin() const
{
	return m_workers.cbegin();
}

WORKER_MGR::CITER WORKER_MGR::cend() const
{
	return m_workers.cend();
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


}
