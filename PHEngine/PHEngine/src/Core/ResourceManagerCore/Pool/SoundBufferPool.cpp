#include "SoundBufferPool.h"

namespace Resources
{
	std::unique_ptr<SoundBufferPool> SoundBufferPool::m_instance;

	std::string SoundBufferPool::ToString() const
	{
		return "SoundBufferPool";
	}

	std::unique_ptr<SoundBufferPool> &SoundBufferPool::GetInstance()
	{
		if (!m_instance)
			m_instance = std::make_unique<SoundBufferPool>();

		return m_instance;
	}

	void SoundBufferPool::ReloadInstance()
	{
		if (m_instance)
			m_instance.reset();
	}
}
