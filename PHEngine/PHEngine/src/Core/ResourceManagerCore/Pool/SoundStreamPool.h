#pragma once

#include "Core/ResourceManagerCore/Pool/PoolBase.h"
#include "Core/AudioCore/SoundStream.h"
#include "Core/ResourceManagerCore/Policy/SoundStreamAllocationPolicy.h"

#include <string>

using namespace EngineCore;

namespace Resources
{
	class SoundStreamPool : public PoolBase<SoundStream, std::string, SoundStreamAllocationPolicy>
	{
		static std::unique_ptr<SoundStreamPool> m_instance;

	public:

		using poolType_t = PoolBase<SoundStream, std::string, SoundStreamAllocationPolicy>;

		std::string ToString() const override;

		static std::unique_ptr<SoundStreamPool>& GetInstance();

		static void ReloadInstance();
	};

}

