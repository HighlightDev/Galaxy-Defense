#pragma once
#include <string>
#include <memory>

#include "Core/AudioCore/SoundMemoryChunk.h"

using namespace EngineCore;

namespace Resources
{
	class SoundMemoryChunkAllocationPolicy
	{
	public:

		static std::shared_ptr<SoundMemoryChunk> AllocateMemory(const std::string& arg);

		static void DeallocateMemory(const std::shared_ptr<SoundMemoryChunk>& arg);
	};

}

