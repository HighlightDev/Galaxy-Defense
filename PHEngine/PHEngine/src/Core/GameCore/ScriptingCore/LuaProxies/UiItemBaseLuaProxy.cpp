#include "UiItemBaseLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiInputSystem/UiMouseInputReceiverBase.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

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
            mCanInterceptMouseInputEvents(ownerUiItem->GetIfCanInterceptMouseInputEvents()),
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

      void UiItemBaseLuaProxy::SetIfCanInterceptMouseInputEvents_FromGameThread(const bool intercepts)
      {
         if (mCanInterceptMouseInputEvents != intercepts)
         {
            mCanInterceptMouseInputEvents = intercepts;
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

      void UiItemBaseLuaProxy::SetAnchor_FromGameThread(const eUiAnchor srcAnchor, const UiAnchorData &uiAnchorData)
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

      void UiItemBaseLuaProxy::SetInputPressState_FromGameThread(const eLuaMouseInputPressState pressState)
      {
         if (mInputPressState != pressState)
         {
            mInputPressState = pressState;
            mIsMouseInputDataDirty = true;
         }
      }

      void UiItemBaseLuaProxy::SetInputCursorHoverState_FromGameThread(const eLuaMouseInputCursorHoverState cursorHoveState)
      {
         if (mInputCursorHoverState != cursorHoveState)
         {
            mInputCursorHoverState = cursorHoveState;
            mIsMouseInputDataDirty = true;
         }
      }

      void UiItemBaseLuaProxy::SetInputClicked_FromGameThread(const bool isClicked)
      {
         if (mInputClicked != isClicked)
         {
            mInputClicked = isClicked;
            mIsMouseInputDataDirty = true;
         }
      }

      void UiItemBaseLuaProxy::SetParent(const std::string &canvasName, const std::string &parentName)
      {
         static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetParent");
         if (const auto sceneSp = mSceneWp.lock())
         {
            const auto replicatorId = GetReplicatorId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, canvasName, parentName]()
                                                                              {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiItemBase = std::static_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                    assert(uiItemBase);
                    uiItemBase->SetParents(canvasName, parentName); });
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
         jsonObj["intercept_mouse_input_event"] = mCanInterceptMouseInputEvents;
         jsonObj["z_order"] = mZOrder;
         jsonObj["width"] = mWidth;
         jsonObj["height"] = mHeight;
         jsonObj["horizontalCenterOffset"] = mHorizontalCenterOffset;
         jsonObj["verticalCenterOffset"] = mVerticalCenterOffset;
         jsonObj["anchors"] = anchorConvertedData;

         mIsLuaDataDirty = false;
         return jsonObj.dump();
      }

      std::string UiItemBaseLuaProxy::GetMouseInputData()
      {
         nlohmann::json jsonObj;
         jsonObj["input_press_state"] = mInputPressState;
         jsonObj["input_cursor_hover_state"] = mInputCursorHoverState;
         jsonObj["input_clicked"] = mInputClicked;
         mInputClicked = false; // due to the fact that we don't receive false for clicked state from game thread

         mIsMouseInputDataDirty = false;
         return jsonObj.dump();
      }

      bool UiItemBaseLuaProxy::IsVisible() const
      {
         return mIsVisible;
      }

      bool UiItemBaseLuaProxy::GetIfCanInterceptMouseInputEvents() const
      {
         return mCanInterceptMouseInputEvents;
      }

      void UiItemBaseLuaProxy::AddAnimation(const std::string &animationName, const AnimationData &animationData)
      {
         static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::AddAnimation");
         if (const auto sceneSp = mSceneWp.lock())
         {
            const auto replicatorId = GetReplicatorId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::PUSH_ANYWAY, mLuaProxyId, functionId, [sceneSp, replicatorId, animationName, animationData]()
                                                                              {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiItemBase = std::dynamic_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                    assert(uiItemBase);
                    uiItemBase->AddAnimation(animationName, animationData); });
         }
      }

      void UiItemBaseLuaProxy::StartAnimation(const std::string &animationName)
      {
         static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::StartAnimation");
         if (const auto sceneSp = mSceneWp.lock())
         {
            const auto replicatorId = GetReplicatorId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, animationName]()
                                                                              {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & uiItemBase = std::dynamic_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                    assert(uiItemBase);
                    const auto& animator = uiItemBase->GetAnimator();
                    assert(animator && animator->HasAnimation(animationName));
                    animator->StartAnimation(animationName); });
         }
      }

      bool UiItemBaseLuaProxy::IsMouseInputDataDirty() const
      {
         return mIsMouseInputDataDirty;
      }

      void UiItemBaseLuaProxy::EnableMouseInputReceiverBase()
      {
         if (mIsMouseInputReceiverEnabled)
            return;

         static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::EnableMouseInputReceiver");
         if (const auto sceneSp = mSceneWp.lock())
         {
            mIsMouseInputReceiverEnabled = true;
            const auto replicatorId = GetReplicatorId();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId]()
                {
                 const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                 assert(replicator);
                 const auto &uiItemBase = std::static_pointer_cast<::EngineCore::GUI::UiItemBase>(replicator);
                 assert(uiItemBase);
                 const auto mouseInputReceiverBase = std::make_shared<UiMouseInputReceiverBase>(uiItemBase);
                 uiItemBase->SetMouseInputReceiver(mouseInputReceiverBase);
                 mouseInputReceiverBase->SetMouseClickedCallback([luaProxyId = uiItemBase->GetLuaProxyId()](const std::weak_ptr<UiItemBase>& eventSender, const glm::ivec2& mousePosition) {
                    if (const auto senderSp = eventSender.lock())
                    {
                       if (!senderSp->IsVisible())
                        return;

                       if (const auto luaScriptProcessorSp = senderSp->GetLuaScriptProcessorWp().lock())
                       {
                          static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetInputClicked");
                          luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, senderSp->GetReplicatorId(), functionId, 
                             [luaScriptProcessorSp, luaProxyId]() {
                             if (const auto& baseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                             {
                                baseLuaProxySp->SetInputClicked_FromGameThread(true);
                             }
                          });
                       }
                    }
                 });

                 mouseInputReceiverBase->SetMousePressedCallback([luaProxyId = uiItemBase->GetLuaProxyId()](const std::weak_ptr<UiItemBase>& eventSender, const glm::ivec2& mousePosition) {
                    if (const auto senderSp = eventSender.lock())
                    {
                       if (!senderSp->IsVisible())
                        return;

                       if (const auto luaScriptProcessorSp = senderSp->GetLuaScriptProcessorWp().lock())
                       {
                          static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetInputPressState");
                          luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, senderSp->GetReplicatorId(), functionId, 
                             [luaScriptProcessorSp, luaProxyId]() {
                             if (const auto& baseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                             {
                                baseLuaProxySp->SetInputPressState_FromGameThread(eLuaMouseInputPressState::MOUSE_BUTTON_PRESSED);
                             }
                          });
                       }
                    }
                 });

                 mouseInputReceiverBase->SetMouseReleasedCallback([luaProxyId = uiItemBase->GetLuaProxyId()](const std::weak_ptr<UiItemBase>& eventSender, const glm::ivec2& mousePosition) {
                    if (const auto senderSp = eventSender.lock())
                    {
                       if (!senderSp->IsVisible())
                           return;

                       if (const auto luaScriptProcessorSp = senderSp->GetLuaScriptProcessorWp().lock())
                       {
                          static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetInputPressState");
                          luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, senderSp->GetReplicatorId(), functionId,
                             [luaScriptProcessorSp, luaProxyId]() {
                             if (const auto& baseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                             {
                                baseLuaProxySp->SetInputPressState_FromGameThread(eLuaMouseInputPressState::MOUSE_BUTTON_RELEASED);
                             }
                          });
                       }
                    }
                 });

                 mouseInputReceiverBase->SetMouseHoverEnteredCallback([luaProxyId = uiItemBase->GetLuaProxyId()](const std::weak_ptr<UiItemBase>& eventSender, const glm::ivec2& mousePosition) {
                    if (const auto senderSp = eventSender.lock())
                    {
                       if (!senderSp->IsVisible())
                           return;

                       if (const auto luaScriptProcessorSp = senderSp->GetLuaScriptProcessorWp().lock())
                       {
                          static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetInputCursorHoverState");
                          luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, senderSp->GetReplicatorId(), functionId,
                             [luaScriptProcessorSp, luaProxyId]() {
                             if (const auto& baseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                             {
                                baseLuaProxySp->SetInputCursorHoverState_FromGameThread(eLuaMouseInputCursorHoverState::CURSOR_HOVER_ENTERED);
                             }
                          });
                       }
                    }
                 });

                 mouseInputReceiverBase->SetMouseHoverLeavedCallback([luaProxyId = uiItemBase->GetLuaProxyId()](const std::weak_ptr<UiItemBase>& eventSender, const glm::ivec2& mousePosition) {
                    if (const auto senderSp = eventSender.lock())
                    {
                       if (!senderSp->IsVisible())
                           return;

                       if (const auto luaScriptProcessorSp = senderSp->GetLuaScriptProcessorWp().lock())
                       {
                          static constexpr auto functionId = Hash64_CT("UiItemBaseLuaProxy::SetInputCursorHoverState");
                          luaScriptProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, senderSp->GetReplicatorId(), functionId,
                             [luaScriptProcessorSp, luaProxyId]() {
                             if (const auto& baseLuaProxySp = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                             {
                                baseLuaProxySp->SetInputCursorHoverState_FromGameThread(eLuaMouseInputCursorHoverState::CURSOR_HOVER_LEAVED);
                             }
                          });
                       }
                    }
                 }); });
         }
      }
   }
}
