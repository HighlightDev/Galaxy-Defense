#include "UiOverlayLuaProxy.h"

#include "Core/GameCore/GUI/OverlayManagement/UiOverlay.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
UiOverlayLuaProxy::UiOverlayLuaProxy(const std::shared_ptr<UiOverlay>& owner)
    : LuaProxy()
    , mOverlayName(owner->GetOverlayName())
{
    mLuaProxyId = owner->GetLuaProxyId();
    SetReplicatorId(owner->GetReplicatorId());
}

std::string UiOverlayLuaProxy::GetOverlayName() const
{
    return mOverlayName;
}

void UiOverlayLuaProxy::OnLuaThreadDataUpdated(const std::string& jsonParameters)
{
}

std::string UiOverlayLuaProxy::GetGameThreadData()
{
    return "";
}
} // namespace Scripts
} // namespace EngineCore
