#include "UiSpaceshipUpgradeItem.h"

using namespace EngineCore;
using namespace EngineCore::GUI;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;

namespace Game {

UiSpaceshipUpgradeItem::UiSpaceshipUpgradeItem(const std::string& name)
    : UiItemBase(name)
{
}

std::shared_ptr<UiSceneProxyBase> UiSpaceshipUpgradeItem::CreateUiSceneProxy() const
{
    return nullptr;
}

std::shared_ptr<LuaProxy> UiSpaceshipUpgradeItem::ReplicateLuaProxy()
{
    return nullptr;
}

void UiSpaceshipUpgradeItem::OnPropertiesShouldBeUpdatedOnRenderThread()
{
}

void UiSpaceshipUpgradeItem::OnPropertiesShouldBeUpdatedOnLuaThread()
{
}

void UiSpaceshipUpgradeItem::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
}

std::string UiSpaceshipUpgradeItem::GetUiTypeString() const
{
    return "UiSpaceshipUpgradeItem";
}

void UiSpaceshipUpgradeItem::OnRegistered()
{
}

void UiSpaceshipUpgradeItem::OnUnregistered()
{
}

} // namespace Game