
#include "workers.hh"

#include <string>
#include <iostream>

namespace WORKERS
{

WORKER_MGR * WORKER_MGR::m_inst = nullptr;

namespace
{

}

WORKER_ENTRY::WORKER_ENTRY(WORKER_NAME && name)
: m_name(std::move(name))
{
};

const WORKER_NAME & WORKER_ENTRY::get_name() const
{
	return m_name;
}

void WORKER_MGR::add_worker(WORKER_ENTRY && worker)
{
	std::cout << "Hello worker " << worker.get_name() << std::endl;
	m_workers.push_back(std::move(worker));
}

WORKER_MGR::CITER WORKER_MGR::cbegin() const
{
	return m_workers.cbegin();
}

WORKER_MGR::CITER WORKER_MGR::cend() const
{
	return m_workers.cend();
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
