#include "FontMeshPool.h"

namespace Resources
{
	std::unique_ptr<FontMeshPool> FontMeshPool::m_instance;

	std::string FontMeshPool::ToString() const
	{
		return "FontMeshPool";
	}

	std::unique_ptr<FontMeshPool> &FontMeshPool::GetInstance()
	{
		if (!m_instance)
			m_instance = std::make_unique<FontMeshPool>();

		return m_instance;
	}

	void FontMeshPool::ReloadInstance()
	{
		if (m_instance)
			m_instance.reset();
	}
}
