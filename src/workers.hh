
#ifndef WORKERS_HH
#define WORKERS_HH

#include <string>

namespace WORKERS
{

class WORKER_ENTRY
{
public:
	WORKER_ENTRY() = delete;
	WORKER_ENTRY(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY(WORKER_ENTRY &&) = default;
	WORKER_ENTRY & operator=(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY & operator=(WORKER_ENTRY &&) = delete;
	~WORKER_ENTRY() = default;


	WORKER_ENTRY(std::string && name);
	std::string get_name() const;

private:
	std::string m_name;

};

class WORKER_MGR
{
public:

	WORKER_MGR & operator=(const WORKER_MGR &) = delete;
	WORKER_MGR & operator=(WORKER_MGR &&) = delete;


	static WORKER_MGR & get_inst();
	void add_worker(WORKER_ENTRY && worker);

private:
	WORKER_MGR() = default;
	WORKER_MGR(const WORKER_MGR &) = delete;
	WORKER_MGR(WORKER_MGR &&) = delete;
	~WORKER_MGR() = default;

	static WORKER_MGR * m_inst;
};


}

#endif
