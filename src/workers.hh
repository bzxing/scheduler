
#ifndef WORKERS_HH
#define WORKERS_HH

#include "jobs.hh"

#include <string>
#include <vector>

namespace WORKERS
{

typedef std::string WORKER_NAME;


class SUBTASK
{
public:
	SUBTASK() = delete;
	SUBTASK(const SUBTASK &) = default;
	SUBTASK(SUBTASK &&) = default;
	SUBTASK & operator=(const SUBTASK &) = delete;
	SUBTASK & operator=(SUBTASK &&) = delete;
	~SUBTASK() = default;

	SUBTASK(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time);

	JOBS::TIME get_start_time() const;
	JOBS::TIME get_complete_time() const; // Defined to be overlapped with next job start time
	const JOBS::JOB_ENTRY & get_job() const;

	std::string to_string() const;

private:
	const JOBS::JOB_ENTRY & m_job;
	JOBS::TIME m_start_time;

};


class WORKER
{
public:
	typedef std::list<SUBTASK> SUBTASK_CONTAINER;
	typedef WORKER::SUBTASK_CONTAINER::iterator SUBTASK_ITER;
	typedef WORKER::SUBTASK_CONTAINER::const_iterator SUBTASK_CITER;

	// Implicit xtors
	WORKER() = delete;
	WORKER(const WORKER &) = delete;
	WORKER(WORKER &&) = default;
	WORKER & operator=(const WORKER &) = delete;
	WORKER & operator=(WORKER &&) = delete;
	~WORKER() = default;

	// Custom ctors
	WORKER(WORKER_NAME && name);

	// Getters
	const WORKER_NAME & get_name() const;
	SUBTASK_CITER cbegin() const;
	SUBTASK_CITER cend() const;
	const SUBTASK_CONTAINER & get_history() const;
	bool execution_history_is_legal() const;
	SUBTASK try_submit_subtask(const JOBS::JOB_ENTRY & job) const;

	// Modifiers
	SUBTASK_CITER submit_subtask(const JOBS::JOB_ENTRY & job);

	// Friends
	friend std::ostream & operator<<(std::ostream & os, const WORKER & worker);

private:
	WORKER_NAME m_name;
	SUBTASK_CONTAINER m_exec_hist;
};
std::ostream & operator<<(std::ostream & os, const WORKER & worker);

class WORKER_MGR
{
private:
	typedef std::vector<WORKER> WORKER_CONTAINER;
public:
	typedef WORKER_CONTAINER::iterator WORKER_ITER;
	typedef WORKER_CONTAINER::const_iterator WORKER_CITER;

	WORKER_MGR & operator=(const WORKER_MGR &) = delete;
	WORKER_MGR & operator=(WORKER_MGR &&) = delete;

	void add_worker(WORKER && worker);
	void submit_job(const JOBS::JOB_ENTRY & job);
	JOBS::TIME get_eta(const JOBS::JOB_ENTRY & job);

	WORKER_ITER begin();
	WORKER_ITER end();

	size_t size() const;
	bool empty() const;

	WORKER_CITER cbegin() const;
	WORKER_CITER cend() const;

	bool execution_history_is_legal() const;
	void verify_execution_history_is_legal() const;

	friend std::ostream & operator<<(std::ostream & os, const WORKER_MGR & worker_mgr);

	static WORKER_MGR & get_inst();

private:
	WORKER_MGR() = default;
	WORKER_MGR(const WORKER_MGR &) = delete;
	WORKER_MGR(WORKER_MGR &&) = delete;
	~WORKER_MGR() = default;

	WORKER_CONTAINER m_workers;

	static WORKER_MGR * m_inst;
};
std::ostream & operator<<(std::ostream & os, const WORKER_MGR & worker_mgr);


}

#endif
