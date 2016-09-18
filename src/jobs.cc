
#include "jobs.hh"

#include <algorithm>

namespace JOBS
{

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


void JOB_QUEUE::add_job(JOB_ENTRY && job)
{
	auto iter = m_jobs.begin();
	for (; iter != m_jobs.end(); ++iter)
	{
		if (l_job_queue_order_less_than(*iter, job))
		{
			break;
		}
	}
	m_jobs.insert(iter, std::move(job));
}


JOB_QUEUE::ITER JOB_QUEUE::begin()
{
	return m_jobs.begin();
}


JOB_QUEUE::ITER JOB_QUEUE::end()
{
	return m_jobs.end();
}





} // End namespace JOBS

