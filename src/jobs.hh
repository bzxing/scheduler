
#ifndef JOBS_HH
#define JOBS_HH

#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <functional>


namespace JOBS
{

typedef std::string JOB_NAME;
typedef size_t PRIORITY;
typedef size_t TIME;

class JOB_STATUS
{
public:
	JOB_STATUS() = default;
	JOB_STATUS(const JOB_STATUS &) = delete;
	JOB_STATUS(JOB_STATUS &&) = default;
	JOB_STATUS & operator=(const JOB_STATUS &) = delete;
	JOB_STATUS & operator=(JOB_STATUS &&) = delete;
	~JOB_STATUS() = default;

	bool submitted() const;
	TIME get_start_time() const;
	TIME get_complete_time() const;

	void set_as_submitted(TIME start, TIME end);

private:
	bool m_submitted = false;
	TIME m_start_time = 0;
	TIME m_complete_time = 0;
};


class JOB_ENTRY
{
public:
	JOB_ENTRY() = delete;
	JOB_ENTRY(const JOB_ENTRY &) = default;
	JOB_ENTRY(JOB_ENTRY &&) = default;
	JOB_ENTRY & operator=(const JOB_ENTRY &) = delete;
	JOB_ENTRY & operator=(JOB_ENTRY &&) = delete;
	~JOB_ENTRY() = default;

	JOB_ENTRY(JOB_NAME && name, PRIORITY pri, size_t num_subtasks,
		TIME earliest_start_time, TIME subtask_duration);

	const JOB_STATUS & get_status() const;
	const JOB_NAME & get_name() const;
	PRIORITY get_priority() const;
	size_t get_num_subtasks() const;
	TIME get_earliest_start_time() const;
	TIME get_subtask_duration() const;

	std::string to_string() const;

private:
	JOB_STATUS m_status;
	JOB_NAME m_name;
	PRIORITY m_priority;
	size_t m_num_subtasks;
	TIME m_earliest_start_time;
	TIME m_subtask_duration;
};


class JOB_QUEUE
{
private:
	typedef std::reference_wrapper<const JOB_ENTRY> JOB_Q_ENTRY;
	typedef std::list<JOB_Q_ENTRY> CONTAINER;
public:
	typedef CONTAINER::iterator ITER;
	typedef CONTAINER::const_iterator CITER;

	JOB_QUEUE & operator=(const JOB_QUEUE &) = delete;
	JOB_QUEUE & operator=(JOB_QUEUE &&) = delete;

	void erase(ITER job_iter);

	ITER begin();
	ITER end();

	CITER cbegin() const;
	CITER cend() const;

	bool empty() const;
	size_t size() const;


	friend std::ostream & operator<<(std::ostream & os, const JOB_QUEUE & job_q);

	static void load();
	static JOB_QUEUE & get_inst();

private:
	JOB_QUEUE();
	JOB_QUEUE(const JOB_QUEUE &) = delete;
	JOB_QUEUE(JOB_QUEUE &&) = delete;
	~JOB_QUEUE() = default;

	void add_job(const JOB_ENTRY & job);

	CONTAINER m_jobs;

	static JOB_QUEUE * m_job_queue_inst;
};

class PARSED_JOBS
{
private:
	typedef std::vector<JOB_ENTRY> CONTAINER;
public:
	typedef CONTAINER::const_iterator CITER;

	PARSED_JOBS & operator=(const PARSED_JOBS &) = delete;
	PARSED_JOBS & operator=(PARSED_JOBS &&) = delete;

	bool empty() const;
	size_t size() const;

	void add_job(JOB_ENTRY && job);

	CITER cbegin() const;
	CITER cend() const;

	friend std::ostream & operator<<(std::ostream & os, const PARSED_JOBS & job_q);

	static PARSED_JOBS & get_inst();

private:
	PARSED_JOBS() = default;
	PARSED_JOBS(const PARSED_JOBS &) = delete;
	PARSED_JOBS(PARSED_JOBS &&) = delete;
	~PARSED_JOBS() = default;

	CONTAINER m_jobs;

	static PARSED_JOBS * m_instance;
};

std::ostream & operator<<(std::ostream & os, const JOB_QUEUE & job_q);
std::ostream & operator<<(std::ostream & os, const PARSED_JOBS & job_q);

}



#endif
