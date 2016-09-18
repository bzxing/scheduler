
#include "workers.hh"

#include <string>
#include <iostream>

namespace WORKERS
{

WORKER_MGR * WORKER_MGR::m_inst = nullptr;

namespace
{

}

WORKER_ENTRY::WORKER_ENTRY(std::string && name)
: m_name(std::move(name))
{
};

std::string WORKER_ENTRY::get_name() const
{
	return m_name;
}

void WORKER_MGR::add_worker(WORKER_ENTRY && worker)
{
	// Use std::move
	std::cout << "Hello worker " << worker.get_name() << std::endl;
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
