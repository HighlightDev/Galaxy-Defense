#pragma once

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <memory>
#include <string>

using namespace Graphics::OpenGL;
using namespace EngineUtility;
using namespace EngineCore;

namespace Resources {

struct ShaderAllocationPolicy {
    template<typename ShaderT, typename ShaderParamsModelType>
    static std::shared_ptr<Shader> AllocateMemory(const ShaderParamsModelType& arg)
    {
        LogInfo("ShaderAllocationPolicy::AllocateMemory: ", arg.ShaderName);
        return std::shared_ptr<Shader>(std::make_shared<ShaderT>(arg));
    }

    static std::shared_ptr<Shader> AllocateMemory(const ShaderParams& arg)
    {
        // to sutisfy Allocatable concept
        ext_assert(false, "ShaderAllocationPolicy::AllocateMemory called without template parameter");
        return nullptr;
    }

    static void DeallocateMemory(std::shared_ptr<Shader> arg)
    {
        LogInfo("ShaderAllocationPolicy::DeallocateMemory: ", arg->GetShaderName());
        arg->CleanUp(true);
    }
};

} // namespace Resources