#include "UiItemLuaProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/GUI/UiElements/UiItem.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
UiItemLuaProxy::UiItemLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItem>& ownerUiItem)
    : UiItemBaseLuaProxy(ownerUiItem)
{
}
} // namespace Scripts
} // namespace EngineCore
