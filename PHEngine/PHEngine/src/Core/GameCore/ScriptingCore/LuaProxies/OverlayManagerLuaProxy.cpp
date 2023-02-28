#include "OverlayManagerLuaProxy.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GameCore/Scene.h"

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        OverlayManagerLuaProxy::OverlayManagerLuaProxy(const std::shared_ptr<OverlayManager> &owner)
            : LuaProxy(),
              mCurrentOverlayName()
        {
            SetReplicatorId(owner->GetReplicatorId());
            owner->SetLuaProxyId(GetLuaProxyId());
        }

        void OverlayManagerLuaProxy::SetCurrentOverlay(const std::string &currentOverlayName)
        {
            mCurrentOverlayName = currentOverlayName;
        }

        std::string OverlayManagerLuaProxy::GetCurrentOverlayName() const
        {
            return mCurrentOverlayName;
        }
    }
}
