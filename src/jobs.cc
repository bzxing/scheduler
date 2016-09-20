
#include "jobs.hh"

#include <algorithm>
#include <iostream>
#include <cassert>

namespace JOBS
{

// Global Variabl Declarations /////////////////////////////////////////////////////////////////////
JOB_QUEUE * JOB_QUEUE::m_job_queue_inst = nullptr;
PARSED_JOBS * PARSED_JOBS::m_instance = nullptr;

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

bool JOB_STATUS::submitted() const
{
	return m_submitted;
}

TIME JOB_STATUS::get_start_time() const
{
	assert(submitted());
	return m_start_time;
}

TIME JOB_STATUS::get_complete_time() const
{
	assert(submitted());
	return m_complete_time;
}

void JOB_STATUS::set_as_submitted(TIME start, TIME end)
{
	assert(!submitted());
	assert(end > start);
	m_submitted = true;
	m_start_time = start;
	m_complete_time = end;
}


JOB_ENTRY::JOB_ENTRY
(
	JOB_NAME && name, PRIORITY pri, size_t num_subtasks,
	TIME earliest_start_time, TIME subtask_duration
) :
	m_name(std::move(name)), m_priority(pri), m_num_subtasks(num_subtasks),
	m_earliest_start_time(earliest_start_time), m_subtask_duration(subtask_duration)
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
	std::string output = m_name;
	output += " stsk=" + std::to_string(m_num_subtasks);
	output += " dur=" + std::to_string(m_subtask_duration);
	output += " erl=" + std::to_string(m_earliest_start_time);
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
	PARSED_JOBS & parsed_jobs = PARSED_JOBS::get_inst();
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
	assert(!PARSED_JOBS::get_inst().empty());
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

void PARSED_JOBS::add_job(JOB_ENTRY && job)
{
	bool debug = true;
	if (debug) std::cout << "Parsed job from input: " << job.get_name() << std::endl;
	m_jobs.push_back(std::move(job));
}

bool PARSED_JOBS::empty() const
{
	return m_jobs.empty();
}

size_t PARSED_JOBS::size() const
{
	return m_jobs.size();
}

PARSED_JOBS::CITER PARSED_JOBS::cbegin() const
{
	return m_jobs.cbegin();
}

PARSED_JOBS::CITER PARSED_JOBS::cend() const
{
	return m_jobs.cend();
}

PARSED_JOBS::ITER PARSED_JOBS::begin()
{
	return m_jobs.begin();
}

PARSED_JOBS::ITER PARSED_JOBS::end()
{
	return m_jobs.end();
}

PARSED_JOBS & PARSED_JOBS::get_inst()
{
	if (m_instance == nullptr)
	{
		m_instance = new PARSED_JOBS;
	}
	return *m_instance;
}

std::ostream & operator<<(std::ostream & os, const PARSED_JOBS & jobs)
{
	for (auto iter = jobs.cbegin(); iter != jobs.cend(); ++iter)
	{
		os << iter->to_string() << std::endl;
	}
	return os;
}


} // End namespace JOBS




