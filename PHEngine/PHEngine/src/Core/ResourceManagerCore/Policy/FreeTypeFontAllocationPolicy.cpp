#include "FreeTypeFontAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FolderManager.h"

using namespace EngineCore;

namespace Resources {
std::shared_ptr<FreeTypeFont> FreeTypeFontAllocationPolicy::AllocateMemory(const std::string& fontFile)
{
    const auto fontFullPathToFile = IO::FolderManager::GetInstance()->GetFontsPath() + fontFile + ".ttf";
    LogInfo("FreeTypeFontAllocationPolicy::AllocateMemory: ", fontFullPathToFile);
    return std::make_shared<FreeTypeFont>(fontFullPathToFile);
}

void FreeTypeFontAllocationPolicy::DeallocateMemory(const std::shared_ptr<FreeTypeFont>& arg)
{
    LogInfo("FreeTypeFontAllocationPolicy::DeallocateMemory");
}

} // namespace Resources
