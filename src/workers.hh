
#ifndef WORKERS_HH
#define WORKERS_HH

#include <string>
#include <vector>

namespace WORKERS
{

typedef std::string WORKER_NAME;

class WORKER_ENTRY
{
public:
	WORKER_ENTRY() = delete;
	WORKER_ENTRY(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY(WORKER_ENTRY &&) = default;
	WORKER_ENTRY & operator=(const WORKER_ENTRY &) = delete;
	WORKER_ENTRY & operator=(WORKER_ENTRY &&) = delete;
	~WORKER_ENTRY() = default;

	WORKER_ENTRY(WORKER_NAME && name);
	const WORKER_NAME & get_name() const;

private:
	WORKER_NAME m_name;

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
