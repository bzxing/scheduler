
#include "jobs.hh"
#include "workers.hh"

#include <algorithm>
#include <iostream>
#include <cassert>

namespace JOBS
{

// Global Variabl Declarations /////////////////////////////////////////////////////////////////////
JOB_QUEUE * JOB_QUEUE::m_job_queue_inst = nullptr;
JOB_POOL * JOB_POOL::m_instance = nullptr;

// Anonymous Namesoace /////////////////////////////////////////////////////////////////////////////
namespace
{

float l_get_priority_weighted_work_load(const JOB_ENTRY & job)
{
	return
		float(job.get_subtask_duration()) * float(job.get_num_subtasks()) / float(job.get_priority());
}

// Ordering policy used by JOB_QUEUE. Returns true if lhs should be placed before rhs.
bool l_job_queue_order_less_than(const JOB_ENTRY & lhs, const JOB_ENTRY & rhs)
{
	if (lhs.get_earliest_start_time() != rhs.get_earliest_start_time())
	{
		return bool(lhs.get_earliest_start_time() < rhs.get_earliest_start_time());
	}
	else
	{
		return bool( l_get_priority_weighted_work_load(lhs) <
			l_get_priority_weighted_work_load(rhs) );
	}
}


} // End anonymous namespace


// Class Routines //////////////////////////////////////////////////////////////////////////////////

JOB_STATUS::JOB_STATUS(JOB_IDX parent_job_idx)
:m_job_idx(parent_job_idx)
{
	reset();
}

const JOB_ENTRY & JOB_STATUS::get_job() const
{
	JOBS::JOB_POOL & job_pool = JOBS::JOB_POOL::get_inst();
	assert(m_job_idx < job_pool.size());
	return job_pool[m_job_idx];
}

void JOB_STATUS::reset()
{
	m_start_time = std::numeric_limits<JOBS::TIME>::max();
	m_complete_time = std::numeric_limits<JOBS::TIME>::min();
	m_subtasks.clear();
}

bool JOB_STATUS::submitted() const
{
	//std::cout << get_job().to_string() << std::endl;
	assert(get_job().get_num_subtasks() > 0);
	return m_subtasks.size() == get_job().get_num_subtasks();
}

bool JOB_STATUS::is_clean() const
{
	return m_subtasks.empty();
}

TIME JOB_STATUS::get_start_time() const
{
	return m_start_time;
}

TIME JOB_STATUS::get_complete_time() const
{
	return m_complete_time;
}

void JOB_STATUS::add_subtask(const WORKERS::SUBTASK & subtask)
{
	m_subtasks.push_back(subtask);
	assert(m_subtasks.size() <= get_job().get_num_subtasks());
	m_start_time = std::min(m_start_time, subtask.get_start_time());
	m_complete_time = std::max(m_complete_time, subtask.get_complete_time());
}


JOB_ENTRY::JOB_ENTRY
(
	JOB_NAME && name, PRIORITY pri, size_t num_subtasks,
	TIME earliest_start_time, TIME subtask_duration, JOB_IDX idx
) :
	m_status(idx), m_name(std::move(name)), m_priority(pri), m_num_subtasks(num_subtasks),
	m_earliest_start_time(earliest_start_time), m_subtask_duration(subtask_duration),
	m_idx(idx)

{

}

const JOB_NAME & JOB_ENTRY::get_name() const
{
	return m_name;
}

const JOB_STATUS & JOB_ENTRY::get_status() const
{
	return m_status;
}

JOB_STATUS & JOB_ENTRY::get_modifiable_status()
{
	return m_status;
}

PRIORITY JOB_ENTRY::get_priority() const
{
	return m_priority;
}

size_t JOB_ENTRY::get_num_subtasks() const
{
	return m_num_subtasks;
}

TIME JOB_ENTRY::get_earliest_start_time() const
{
	return m_earliest_start_time;
}

TIME JOB_ENTRY::get_subtask_duration() const
{
	return m_subtask_duration;
}

std::string JOB_ENTRY::to_string() const
{
	bool submitted = get_status().submitted();
	std::string output;
	output += "#" + std::to_string(m_idx) + " " + m_name;
	output += " sbtk=" + std::to_string(m_num_subtasks);
	output += " dur=" + std::to_string(m_subtask_duration);
	output += " erly=" + std::to_string(m_earliest_start_time);
	output += " pri=" + std::to_string(m_priority);
	output += " sbmt=" + std::to_string(submitted);
	if (submitted)
	{
		output += " start=" + std::to_string(get_status().get_start_time());
		output += " end=" + std::to_string(get_status().get_complete_time());
	}
	return output;

}

JOB_QUEUE::JOB_QUEUE()
{
	JOB_POOL & parsed_jobs = JOB_POOL::get_inst();
	assert(!parsed_jobs.empty());
	for (auto iter = parsed_jobs.begin(); iter != parsed_jobs.end(); ++iter)
	{
		add_job(*iter);
	}
}

void JOB_QUEUE::add_job(JOB_ENTRY & job)
{
	JOB_Q_ENTRY new_entry(job);

	bool debug = false;
	if (debug) std::cout << "Queuing job " << job.get_name() << std::endl;
	auto iter = std::upper_bound(
		m_jobs.begin(), m_jobs.end(), new_entry,
		[](const JOB_Q_ENTRY & lhs, const JOB_Q_ENTRY & rhs) {
			return bool(l_job_queue_order_less_than(lhs.get(), rhs.get()));
		});
	m_jobs.insert(iter, std::move(new_entry));
}


void JOB_QUEUE::erase(JOB_QUEUE::ITER job_iter)
{
	m_jobs.erase(job_iter);
}

JOB_QUEUE::ITER JOB_QUEUE::begin()
{
	return m_jobs.begin();
}


JOB_QUEUE::ITER JOB_QUEUE::end()
{
	return m_jobs.end();
}

JOB_QUEUE::CITER JOB_QUEUE::cbegin() const
{
	return m_jobs.cbegin();
}

JOB_QUEUE::CITER JOB_QUEUE::cend() const
{
	return m_jobs.cend();
}

bool JOB_QUEUE::empty() const
{
	return m_jobs.empty();
}

size_t JOB_QUEUE::size() const
{
	return m_jobs.size();
}

void JOB_QUEUE::load()
{
	bool debug = true;

	if (debug) std::cout << "Loading up job queue...\n";

	assert(m_job_queue_inst == nullptr);
	assert(!JOB_POOL::get_inst().empty());
	m_job_queue_inst = new JOB_QUEUE;
	assert(m_job_queue_inst != nullptr);
	assert(!m_job_queue_inst->empty());

	if (debug)
	{
		std::cout << "Done loading job queue! Here's the result in order:\n";
		std::cout << *m_job_queue_inst;
	}

}


JOB_QUEUE & JOB_QUEUE::get_inst()
{
	assert(m_job_queue_inst != nullptr);
	return *m_job_queue_inst;
}

std::ostream & operator<<(std::ostream & os, const JOB_QUEUE & job_q)
{
	for (auto iter = job_q.cbegin(); iter != job_q.cend(); ++iter)
	{
		os << iter->get().to_string() << std::endl;
	}
	return os;
}

void JOB_POOL::add_job(JOB_ENTRY && job)
{
	bool debug = true;
	if (debug) std::cout << "Parsed job from input: " << job.get_name() << std::endl;
	m_jobs.push_back(std::move(job));
}

bool JOB_POOL::empty() const
{
	return m_jobs.empty();
}

size_t JOB_POOL::size() const
{
	return m_jobs.size();
}

JOB_POOL::CITER JOB_POOL::cbegin() const
{
	return m_jobs.cbegin();
}

JOB_POOL::CITER JOB_POOL::cend() const
{
	return m_jobs.cend();
}

JOB_POOL::ITER JOB_POOL::begin()
{
	return m_jobs.begin();
}

JOB_POOL::ITER JOB_POOL::end()
{
	return m_jobs.end();
}

JOB_POOL & JOB_POOL::get_inst()
{
	if (m_instance == nullptr)
	{
		m_instance = new JOB_POOL;
	}
	return *m_instance;
}

std::ostream & operator<<(std::ostream & os, const JOB_POOL & jobs)
{
	for (auto iter = jobs.cbegin(); iter != jobs.cend(); ++iter)
	{
		os << iter->to_string() << std::endl;
	}
	return os;
}


} // End namespace JOBS




