#include "LuaCommonUiFunctions.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/ReplicatorFactories/CommonUiWidgetType.h"
#include "Core/GameCore/ScriptingCore/ReplicatorFactories/CommonUiWidgetFactoryCreator.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiCanvasLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiItemBaseLuaProxy.h"

using namespace EngineCore;
using namespace IO;

namespace EngineCore
{
   namespace Scripts
   {
      LuaCommonUiFunctions::LuaCommonUiFunctions(LuaScriptExecutorBase *ownerPtr)
          : mOwnerPtr(ownerPtr)
      {
      }

      void LuaCommonUiFunctions::SetScene(const std::weak_ptr<Scene> &sceneWp)
      {
         mSceneWp = sceneWp;
      }

      void LuaCommonUiFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor)
      {
         mLuaScriptProcessor = scriptProcessor;
      }

      void LuaCommonUiFunctions::OnScriptStarted(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiFunctions::OnScriptStopped(const LuaWrapper &luaWrapper)
      {
      }

      void LuaCommonUiFunctions::RegisterCallbacks(const LuaWrapper &luaWrapper)
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetCurrentOverlayName"), std::string(void)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::GetCurrentOverlayName, this, std::placeholders::_1), "_GetCurrentOverlayName");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OpenOverlay"), void(std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::OpenOverlay, this, std::placeholders::_1), "_OpenOverlay");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CloseCurrentOverlay"), void()>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::CloseCurrentOverlay, this, std::placeholders::_1), "_CloseCurrentOverlay");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::CreateCommonUiWidget"), int32_t(int32_t, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::CreateCommonUiWidget, this, std::placeholders::_1), "_CreateCommonUiWidget");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::IsLuaProxyReady"), bool(int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::IsLuaProxyReady, this, std::placeholders::_1), "_IsLuaProxyReady");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated"), void(int32_t, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated, this, std::placeholders::_1), "_OnCommonUiWidgetDataUpdated");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetGameThreadData"), std::string(int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::GetGameThreadData, this, std::placeholders::_1), "_GetGameThreadData");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::InitializeCanvasInputSystem"), void(int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::InitializeCanvasInputSystem, this, std::placeholders::_1), "_InitializeCanvasInputSystem");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::GetUiWidgetName"), std::string(int32_t)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::GetUiWidgetName, this, std::placeholders::_1), "_GetUiWidgetName");
         LuaCallbackBindingHelper<Hash64_CT("LuaCommonUiFunctions::SetUiWidgetParent"), void(int32_t, std::string, std::string)>::Bind(luaWrapper, mOwnerPtr, std::bind(&LuaCommonUiFunctions::SetUiWidgetParent, this, std::placeholders::_1), "_SetUiWidgetParent");
      }

      std::string LuaCommonUiFunctions::GetCurrentOverlayName(const std::tuple<> &data)
      {
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            const auto overlayName = luaProcessorSp->GetOverlayManagerLuaProxy()->GetCurrentOverlayName();
            LogInfo("LuaCommonUiFunctions::GetCurrentOverlayName => overlayName:", overlayName);
            return overlayName;
         }
         LogInfo("LuaCommonUiFunctions::GetCurrentOverlayName => FAILED!");

         return "";
      }

      void LuaCommonUiFunctions::OpenOverlay(const std::tuple<std::string> &overlayName)
      {
         const auto &overlayToOpen = std::get<0>(overlayName);

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            luaProcessorSp->GetOverlayManagerLuaProxy()->OpenOverlay(overlayToOpen);
         }
      }

      void LuaCommonUiFunctions::CloseCurrentOverlay(const std::tuple<> &emptyData)
      {
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            luaProcessorSp->GetOverlayManagerLuaProxy()->CloseCurrentOverlay();
         }
      }

      int32_t LuaCommonUiFunctions::CreateCommonUiWidget(const std::tuple<int32_t, std::string> &data)
      {
         const auto commonUiWidgetType = static_cast<eCommonUiWidgetType>(std::get<0>(data));
         const auto &jsonParametersStr = std::get<1>(data);
         CommonUiWidgetFactoryCreator factoryCreator;
         const auto &replicatorFactory = factoryCreator.GetReplicatorFactory(commonUiWidgetType);
         const int32_t luaProxyId = replicatorFactory->CreateReplicator(mSceneWp, mLuaScriptProcessor, jsonParametersStr);
         LogInfo("LuaCommonUiFunctions::CreateCommonUiWidget => widgetType: ", static_cast<uint8_t>(commonUiWidgetType), ", luaProxyId: ", luaProxyId);
         return luaProxyId;
      }

      bool LuaCommonUiFunctions::IsLuaProxyReady(const std::tuple<int32_t> &data)
      {
         const auto luaProxyId = std::get<0>(data);

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            return nullptr != luaProcessorSp->GetLuaProxy(luaProxyId);
         }

         return false;
      }

      std::string LuaCommonUiFunctions::GetUiWidgetName(const std::tuple<int32_t> &data)
      {
         const auto luaProxyId = std::get<0>(data);
         std::string widgetName;

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            if (const auto &luaProxy = luaProcessorSp->GetLuaProxy(luaProxyId))
            {
               if (std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProxy))
               {
                  const auto &uiItemWidgetProxy = std::static_pointer_cast<UiItemBaseLuaProxy>(luaProxy);
                  widgetName = uiItemWidgetProxy->GetUiItemName();
               }
               else if (std::dynamic_pointer_cast<UiCanvasLuaProxy>(luaProxy))
               {
                  const auto &uiCanvasProxy = std::static_pointer_cast<UiCanvasLuaProxy>(luaProxy);
                  widgetName = uiCanvasProxy->GetCanvasName();
               }
               else
               {
                  assert(false);
               }
            }
         }

         return widgetName;
      }

      void LuaCommonUiFunctions::SetUiWidgetParent(const std::tuple<int32_t /*lua proxy id*/, std::string /*canvas name*/, std::string /*parent name*/> &data)
      {
         const auto luaProxyId = std::get<0>(data);
         const auto canvasName = std::get<1>(data);
         const auto parentName = std::get<2>(data);

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            if (const auto &luaProxy = luaProcessorSp->GetLuaProxy(luaProxyId))
            {
               const auto &uiItemWidgetProxy = std::dynamic_pointer_cast<UiItemBaseLuaProxy>(luaProxy);
               assert(uiItemWidgetProxy);
               uiItemWidgetProxy->SetParent(canvasName, parentName);
            }
         }
      }

      void LuaCommonUiFunctions::OnCommonUiWidgetDataUpdated(const std::tuple<int32_t /*lua proxy id*/, std::string /*json data*/> &data)
      {
         const auto luaProxyId = std::get<0>(data);
         const auto &jsonDataStr = std::get<1>(data);

         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            if (const auto &luaProxySp = luaProcessorSp->GetLuaProxy(luaProxyId))
            {
               luaProxySp->OnLuaThreadDataUpdated(jsonDataStr);
            }
         }
      }

      std::string LuaCommonUiFunctions::GetGameThreadData(const std::tuple<int32_t /*lua proxy id*/> &data)
      {
         const auto luaProxyId = std::get<0>(data);
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            if (const auto &luaProxySp = luaProcessorSp->GetLuaProxy(luaProxyId))
            {
               if (luaProxySp->IsLuaDataDirty())
               {
                  return luaProxySp->GetGameThreadData();
               }
            }
         }
         return "";
      }

      void LuaCommonUiFunctions::InitializeCanvasInputSystem(const std::tuple<int32_t /*lua proxy id*/> &data)
      {
         const auto luaProxyId = std::get<0>(data);
         if (const auto &luaProcessorSp = mOwnerPtr->GetLuaScriptProcessor().lock())
         {
            const auto &canvasSp = std::dynamic_pointer_cast<UiCanvasLuaProxy>(luaProcessorSp->GetLuaProxy(luaProxyId));
            assert(canvasSp);
            canvasSp->InitializeInputSystem();
         }
      }
   }
}
