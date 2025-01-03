#include "UiRowLayout.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRowLayoutLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace EngineCore
{
    namespace GUI
    {

        UiRowLayout::UiRowLayout(const std::string &name)
            : UiItem(name)
        {
        }

        void UiRowLayout::UpdateAnchorTransform()
        {
            UiItem::UpdateAnchorTransform();

            RecalculatePositionsForChildren();
        }

        void UiRowLayout::SetSpacing(const uint32_t spacing)
        {
            assert(mSpacing < mWidth);
            if (mSpacing != spacing)
            {
                mSpacing = spacing;
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
                SetIsTransformDirty(true);
            }
        }

        uint32_t UiRowLayout::GetSpacing() const
        {
            return mSpacing;
        }

        void UiRowLayout::RecalculatePositionsForChildren()
        {
            if (mChildren.size() && mWidth > 0 && mHeight > 0)
            {
                const auto spacingsCount = mChildren.size() - 1;
                // auto potentialAccumulatedWidthOfChildren = std::accumulate(mChildren.cbegin(), mChildren.cend(), 0, [](const int32_t total, const std::shared_ptr<UiItemBase> &child)
                //                                                            { return child->GetWidth() + total; });

                // potentialAccumulatedWidthOfChildren += spacingsCount * mSpacing;
                const uint32_t normalizedChildWidth = static_cast<uint32_t>((mWidth - (spacingsCount * mSpacing)) / mChildren.size());

                int32_t childIndex = 0;
                uint32_t childPositionXCursor = mAbsoluteOrigin.x;
                for (const auto &child : mChildren)
                {
                    const auto &anchors = child->GetAnchors();
                    ext_assert(anchors.size() == 0, "Ui widget cannot have anchors inside layout widget.");

                    // const auto childWidth = potentialAccumulatedWidthOfChildren > mWidth ? normalizedChildWidth : child->GetWidth();
                    const auto childWidth = normalizedChildWidth;
                    child->SetWidth(childWidth);
                    child->SetAbsoluteOrigin(glm::ivec2(childPositionXCursor, mAbsoluteOrigin.y));

                    childPositionXCursor += childWidth + mSpacing;
                }
            }
        }

        std::shared_ptr<LuaProxy> UiRowLayout::ReplicateLuaProxy()
        {
            return std::make_shared<UiRowLayoutLuaProxy>(std::static_pointer_cast<UiRowLayout>(shared_from_this()));
        }

        std::string UiRowLayout::GetUiTypeString() const
        {
            return "UiRowLayout";
        }

        void UiRowLayout::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            UiItem::SyncFromLuaJsonProperties(luaJsonPropsStr);

            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("spacing"))
            {
                const auto spacing = nlohmann_utilities::GetIntFromJson(jsonObj["spacing"]);
                if (spacing != mSpacing)
                {
                    mSpacing = spacing;
                    SetIsTransformDirty(true);
                }
            }
        }

        void UiRowLayout::OnPropertiesShouldBeUpdatedOnLuaThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

            SyncDataOnLuaThread();
        }

        void UiRowLayout::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiRowLayout::SyncDataOnLuaThread");
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), spacing = mSpacing]()
                                                                                         {
                            if (const auto &layoutLuaProxy = std::static_pointer_cast<UiRowLayoutLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                            {
                                layoutLuaProxy->SetSpacing_FromGameThread(spacing);
                            } });
                    }
                }
            }
        }
    }
}
