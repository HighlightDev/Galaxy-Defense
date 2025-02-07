#pragma once

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"

#include <memory>
#include <vector>

using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Resources {
struct CompositeShaderAllocationPolicy {
    template<typename CompositeShaderType>
    static std::shared_ptr<IShader> AllocateMemory(const CompositeShaderParams& arg)
    {
        LogInfo("CompositeShaderAllocationPolicy::AllocateMemory: ", arg.mShaderName);
        return std::make_shared<CompositeShaderType>(arg);
    }

    static void DeallocateMemory(std::shared_ptr<IShader> arg)
    {
        LogInfo("CompositeShaderAllocationPolicy::DeallocateMemory");
        arg->CleanUp(true);
    }
};

} // namespace Resources