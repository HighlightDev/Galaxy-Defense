#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/ResourceManagerCore/Policy/ShaderAllocationPolicy.h"

#include <string>

using namespace Graphics::OpenGL;

namespace Resources
{

	class ShaderPool : public PoolBase<Shader, ShaderParams, ShaderAllocationPolicy>
	{
		static std::unique_ptr<ShaderPool> m_instance;

	public:
		using poolType_t = PoolBase<Shader, ShaderParams, ShaderAllocationPolicy>;

		std::string ToString() const override;

		static std::unique_ptr<ShaderPool> &GetInstance();

		static void ReloadInstance();
#if DEBUG
		void RecompileShaders();
#endif
	};
}
