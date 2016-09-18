
#ifndef JOBS_HH
#define JOBS_HH

#include <string>
#include <cstring>
#include <list>


namespace JOBS
{

typedef std::string JOB_NAME;
typedef size_t PRIORITY;
typedef size_t TIME;


class JOB_ENTRY
{
public:
	JOB_ENTRY() = delete;
	JOB_ENTRY(const JOB_ENTRY &) = delete;
	JOB_ENTRY(JOB_ENTRY &&) = default;
	JOB_ENTRY & operator=(const JOB_ENTRY &) = delete;
	JOB_ENTRY & operator=(JOB_ENTRY &&) = delete;
	~JOB_ENTRY() = default;

	JOB_ENTRY(JOB_NAME && name, PRIORITY pri, size_t num_subtasks,
		TIME earliest_start_time, TIME subtask_duration);

	const JOB_NAME & get_name() const;
	PRIORITY get_priority() const;
	size_t get_num_subtasks() const;
	TIME get_earliest_start_time() const;
	TIME get_subtask_duration() const;

	std::string to_string() const;

private:
	JOB_NAME m_name;
	PRIORITY m_priority;
	size_t m_num_subtasks;
	TIME m_earliest_start_time;
	TIME m_subtask_duration;
};


class JOB_QUEUE
{
private:
	typedef std::list<JOB_ENTRY> CONTAINER;
public:
	typedef CONTAINER::iterator ITER;
	typedef CONTAINER::const_iterator CITER;

	JOB_QUEUE & operator=(const JOB_QUEUE &) = delete;
	JOB_QUEUE & operator=(JOB_QUEUE &&) = delete;

	void add_job(JOB_ENTRY && job);
	void erase(ITER job_iter);

	ITER begin();
	ITER end();

	CITER cbegin() const;
	CITER cend() const;

	bool empty() const;
	size_t size() const;


	friend std::ostream & operator<<(std::ostream & os, const JOB_QUEUE & job_q);

	static JOB_QUEUE & get_inst();

private:
	JOB_QUEUE() = default;
	JOB_QUEUE(const JOB_QUEUE &) = delete;
	JOB_QUEUE(JOB_QUEUE &&) = delete;
	~JOB_QUEUE() = default;

	CONTAINER m_jobs;

	static JOB_QUEUE * m_job_queue_inst;
};

std::ostream & operator<<(std::ostream & os, const JOB_QUEUE & job_q);

}



#endif
