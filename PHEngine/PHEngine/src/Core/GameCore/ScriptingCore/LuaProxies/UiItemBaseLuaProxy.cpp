#include "UiItemBaseLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        UiItemBaseLuaProxy::UiItemBaseLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiItemBase> &ownerUiItem)
            : LuaProxy(),
              mUiItemName(ownerUiItem->GetName()),
              mIsVisible(ownerUiItem->IsVisible()),
              mZOrder(ownerUiItem->GetZOrder()),
              mWidth(ownerUiItem->GetWidth()),
              mHeight(ownerUiItem->GetHeight()),
              mHorizontalCenterOffset(ownerUiItem->GetHorizontalCenterOffset()),
              mVerticalCenterOffset(ownerUiItem->GetVerticalCenterOffset())
        {
            mLuaProxyId = ownerUiItem->GetLuaProxyId();
            SetReplicatorId(ownerUiItem->GetReplicatorId());
        }

        std::string UiItemBaseLuaProxy::GetUiItemName() const
        {
            return mUiItemName;
        }

        void UiItemBaseLuaProxy::SetIsVisible_FromGameThread(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetZOrder_FromGameThread(const size_t zOrder)
        {
            if (mZOrder != zOrder)
            {
                mZOrder = zOrder;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetWidth_FromGameThread(const size_t width)
        {
            if (mWidth != width)
            {
                mWidth = width;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetHeight_FromGameThread(const size_t height)
        {
            if (mHeight != height)
            {
                mHeight = height;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetAnchor_FromGameThread(const eUiAnchor srcAnchor, const UiAnchorData& uiAnchorData)
        {
            mAnchors[srcAnchor] = uiAnchorData;
        }

        void UiItemBaseLuaProxy::SetHorizontalCenterOffset_FromGameThread(const int32_t horizontalCenterOffset)
        {
            if (mHorizontalCenterOffset != horizontalCenterOffset)
            {
                mHorizontalCenterOffset = horizontalCenterOffset;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetVerticalCenterOffset_FromGameThread(const int32_t verticalCenterOffset)
        {
            if (mVerticalCenterOffset != verticalCenterOffset)
            {
                mVerticalCenterOffset = verticalCenterOffset;
                mIsLuaDataDirty = true;
            }
        }

        void UiItemBaseLuaProxy::SetParent(const std::string& canvasName, const std::string &parentName)
        {
            static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetParent");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, canvasName, parentName]() {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiItemBase = std::static_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                    assert(uiItemBase);
                    uiItemBase->SetParents(canvasName, parentName);
                });
            }
        }

        void UiItemBaseLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
            static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::OnLuaThreadDataUpdated");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, jsonStr = jsonParameters]()
                    {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiItemBase = std::static_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                    assert(uiItemBase);
                    uiItemBase->SyncFromLuaJsonProperties(jsonStr); });
            }
        }

        std::string UiItemBaseLuaProxy::GetGameThreadData()
        {
            std::unordered_map<eUiAnchor, std::tuple<eUiAnchor /*dst anchor*/, std::string /* dstUiItemName*/, int32_t /*anchor margin*/>> anchorConvertedData;
            std::transform(mAnchors.cbegin(), mAnchors.cend(),
                           std::inserter(anchorConvertedData, anchorConvertedData.begin()),
                           [](const auto &pair)
                           {
                               return std::make_pair(pair.first, std::make_tuple(pair.second.GetDstAnchor(), pair.second.GetDstUiItemName(), pair.second.GetSrcAnchorMargin()));
                           });
            nlohmann::json jsonObj;
            jsonObj["visible"] = mIsVisible;
            jsonObj["z_order"] = mZOrder;
            jsonObj["width"] = mWidth;
            jsonObj["height"] = mHeight;
            jsonObj["horizontalCenterOffset"] = mHorizontalCenterOffset;
            jsonObj["verticalCenterOffset"] = mVerticalCenterOffset;
            jsonObj["anchors"] = anchorConvertedData;

            mIsLuaDataDirty = false;
            return jsonObj.dump();
        }

        bool UiItemBaseLuaProxy::IsVisible() const
        {
            return mIsVisible;
        }
    }
}
