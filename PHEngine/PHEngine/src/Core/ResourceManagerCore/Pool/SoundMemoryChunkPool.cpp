#include "SoundMemoryChunkPool.h"

namespace Resources
{
	std::unique_ptr<SoundMemoryChunkPool> SoundMemoryChunkPool::m_instance;

	std::string SoundMemoryChunkPool::ToString() const
	{
		return "SoundMemoryChunkPool";
	}

	std::unique_ptr<SoundMemoryChunkPool> &SoundMemoryChunkPool::GetInstance()
	{
		if (!m_instance)
			m_instance = std::make_unique<SoundMemoryChunkPool>();

		return m_instance;
	}

	void SoundMemoryChunkPool::ReloadInstance()
	{
		if (m_instance)
			m_instance.reset();
	}
}
