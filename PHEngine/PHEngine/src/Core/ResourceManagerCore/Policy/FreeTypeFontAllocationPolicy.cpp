#include "FreeTypeFontAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Resources {
std::shared_ptr<FreeTypeFont> FreeTypeFontAllocationPolicy::AllocateMemory(const std::string& fontFile)
{
    LogInfo("FreeTypeFontAllocationPolicy::AllocateMemory: ", fontFile);
    return std::make_shared<FreeTypeFont>(fontFile);
}

void FreeTypeFontAllocationPolicy::DeallocateMemory(const std::shared_ptr<FreeTypeFont>& arg)
{
    LogInfo("FreeTypeFontAllocationPolicy::DeallocateMemory");
}

} // namespace Resources
