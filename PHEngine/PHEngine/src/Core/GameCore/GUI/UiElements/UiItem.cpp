#include "UiItem.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineCore;

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
    }
}
