
#ifndef WORKERS_HH
#define WORKERS_HH

#include "jobs.hh"

#include <string>
#include <vector>

namespace WORKERS
{

typedef std::string WORKER_NAME;

class EXEC_HISTORY_ENTRY
{
public:
	EXEC_HISTORY_ENTRY() = delete;
	EXEC_HISTORY_ENTRY(const EXEC_HISTORY_ENTRY &) = delete;
	EXEC_HISTORY_ENTRY(EXEC_HISTORY_ENTRY &&) = default;
	EXEC_HISTORY_ENTRY & operator=(const EXEC_HISTORY_ENTRY &) = delete;
	EXEC_HISTORY_ENTRY & operator=(EXEC_HISTORY_ENTRY &&) = delete;
	~EXEC_HISTORY_ENTRY() = default;

	EXEC_HISTORY_ENTRY(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time);

	JOBS::TIME get_start_time() const;
	JOBS::TIME get_complete_time() const;
	const JOBS::JOB_ENTRY & get_job() const;

private:
	JOBS::JOB_ENTRY m_job;
	JOBS::TIME m_start_time;

};


class WORKER_ENTRY
{
private:
	typedef std::vector<EXEC_HISTORY_ENTRY> EXEC_HISTORY;
	typedef EXEC_HISTORY::const_iterator CITER;
public:
	WORKER_ENTRY() = delete;
	WORKER_ENTRY(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY(WORKER_ENTRY &&) = default;
	WORKER_ENTRY & operator=(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY & operator=(WORKER_ENTRY &&) = delete;
	~WORKER_ENTRY() = default;

	WORKER_ENTRY(WORKER_NAME && name);

	const WORKER_NAME & get_name() const;
	CITER cbegin() const;
	CITER cend() const;

	void append_job(const JOBS::JOB_ENTRY & job, JOBS::TIME start_time);

private:
	WORKER_NAME m_name;
	EXEC_HISTORY m_exec_hist;
};

class WORKER_MGR
{
private:
	typedef std::vector<WORKER_ENTRY> WORKER_CONTAINER;
public:
	typedef WORKER_CONTAINER::const_iterator CITER;

	WORKER_MGR & operator=(const WORKER_MGR &) = delete;
	WORKER_MGR & operator=(WORKER_MGR &&) = delete;

	void add_worker(WORKER_ENTRY && worker);
	void submit_job(const JOBS::JOB_ENTRY & job);
	JOBS::TIME get_eta(const JOBS::JOB_ENTRY & job);

	size_t size() const;
	bool empty() const;

	CITER cbegin() const;
	CITER cend() const;

	static WORKER_MGR & get_inst();

private:
	WORKER_MGR() = default;
	WORKER_MGR(const WORKER_MGR &) = delete;
	WORKER_MGR(WORKER_MGR &&) = delete;
	~WORKER_MGR() = default;

	WORKER_CONTAINER m_workers;

	static WORKER_MGR * m_inst;
};


}

#endif
