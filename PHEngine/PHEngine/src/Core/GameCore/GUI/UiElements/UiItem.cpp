#include "UiItem.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiItemLuaProxy.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace EngineCore
{
    namespace GUI
    {

        UiItem::UiItem(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(parentCanvas, parent)
        {
        }

        void UiItem::OnRegistered()
        {

        }

        void UiItem::OnUnregistered()
        {

        }

        std::shared_ptr<LuaProxy> UiItem::ReplicateLuaProxy()
        {
            return std::make_shared<UiItemLuaProxy>(std::static_pointer_cast<UiItem>(shared_from_this()));
        }
    }
}
