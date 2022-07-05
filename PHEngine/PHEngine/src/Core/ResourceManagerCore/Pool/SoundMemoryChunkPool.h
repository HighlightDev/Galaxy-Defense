#pragma once

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/AudioCore/SoundMemoryChunk.h"
#include "Core/ResourceManagerCore/Policy/SoundMemoryChunkAllocationPolicy.h"

#include <string>

using namespace EngineCore;

namespace Resources
{
	class SoundMemoryChunkPool : public PoolBase<SoundMemoryChunk, std::string, SoundMemoryChunkAllocationPolicy>
	{
		static std::unique_ptr<SoundMemoryChunkPool> m_instance;

	public:

		using poolType_t = PoolBase<SoundMemoryChunk, std::string, SoundMemoryChunkAllocationPolicy>;

		virtual std::string ToString() const override;

		static std::unique_ptr<SoundMemoryChunkPool>& GetInstance();

		static void ReloadInstance();
	};

}

