#include "MeshPool.h"

namespace Resources
{
	std::unique_ptr<MeshPool> MeshPool::m_instance;

	std::string MeshPool::ToString() const
	{
		return "MeshPool";
	}

	std::unique_ptr<MeshPool> &MeshPool::GetInstance()
	{
		if (!m_instance)
			m_instance = std::make_unique<MeshPool>();

		return m_instance;
	}

	void MeshPool::ReloadInstance()
	{
		if (m_instance)
			m_instance.reset();
	}

}
