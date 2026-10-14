#include "ResolveFxColorShader.h"

#include "Core/GraphicsCore/OpenGL/Shader/ShaderType.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <functional>

using namespace EngineUtility;
using namespace Graphics;

namespace EngineCore {
namespace ShaderImpl {
ResolveFxColorShader::ResolveFxColorShader(const ShaderParams& params)
    : Shader(params)
{
    ShaderInit();
}

ResolveFxColorShader::~ResolveFxColorShader()
{
}

void ResolveFxColorShader::SetSceneColorTexture(const int32_t textureSlot)
{
    u_sceneColorTexture.LoadUniform(textureSlot);
}

void ResolveFxColorShader::SetBloomColorTexture(const int32_t textureSlot)
{
    u_bloomColorTexture.LoadUniform(textureSlot);
}

void ResolveFxColorShader::SetResolveAlphaFromSource(const bool resolveAlphaFromSource)
{
    u_resolveAlphaFromSource.LoadUniform(resolveAlphaFromSource ? 1.0f : 0.0f);
}

void ResolveFxColorShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    Shader::AccessAllUniformLocations(shaderProgramID);

    u_sceneColorTexture = GetUniform("sceneColorTexture", shaderProgramID);
    u_bloomColorTexture = GetUniform("bloomColorTexture", shaderProgramID);
    u_resolveAlphaFromSource = GetUniform("resolveAlphaFromSource", shaderProgramID);
}

void ResolveFxColorShader::SetShaderPredefine()
{
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    const bool isHdrEnabled = cfg.IsHdrEnabled;
    const bool isBloomEnabled = cfg.IsBloomEnabled;

    const auto& hdrDefineFunc = std::mem_fn(isHdrEnabled ? &ResolveFxColorShader::Define : &ResolveFxColorShader::Undefine);
    const auto& bloomDefineFunc = std::mem_fn(isBloomEnabled ? &ResolveFxColorShader::Define : &ResolveFxColorShader::Undefine);
    hdrDefineFunc(*this, eShaderType::FragmentShader, "HDR_ON");
    bloomDefineFunc(*this, eShaderType::FragmentShader, "BLOOM_ON");
    DefineConstant<float>(eShaderType::FragmentShader, "HDR_EXPOSURE", cfg.HdrExposureValue);
}
} // namespace ShaderImpl
} // namespace EngineCore