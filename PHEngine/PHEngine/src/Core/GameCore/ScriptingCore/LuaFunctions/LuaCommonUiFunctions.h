#pragma once
#include <tuple>
#include <string>

#include "ILuaFunctionable.h"

namespace EngineCore
{
   class Scene;
}

namespace EngineCore
{
   namespace Scripts
   {
      class LuaScriptExecutorBase;
      class LuaScriptProcessor;

      class LuaCommonUiFunctions
          : public ILuaFunctionable
      {
      protected:
         LuaScriptExecutorBase *mOwnerPtr;

         std::weak_ptr<::EngineCore::Scene> mSceneWp;

         std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

      public:
         LuaCommonUiFunctions(LuaScriptExecutorBase *ownerPtr);

         void Initialize() override;

         void SetScene(const std::weak_ptr<Scene> &sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) override;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

      private:
         std::string GetCurrentOverlayName(const std::tuple<> &data);

         void OpenOverlay(const std::tuple<std::string> &overlayName);

         void OpenBackgroundOverlay(const std::tuple<std::string> &overlayName);

         void CloseCurrentOverlay(const std::tuple<> &emptyData);

         void CloseBackgroundOverlay(const std::tuple<std::string> &emptyData);

         int32_t CreateCommonUiWidget(const std::tuple<int32_t, std::string> &data);

         bool IsLuaProxyReady(const std::tuple<int32_t> &data);

         std::string GetUiWidgetName(const std::tuple<int32_t> &data);

         void OnCommonUiWidgetDataUpdated(const std::tuple<int32_t /*lua proxy id*/, std::string /*json data*/> &data);

         std::string GetGameThreadData(const std::tuple<int32_t /*lua proxy id*/> &data);

         std::string GetMouseInputData(const std::tuple<int32_t /*lua proxy id*/> &data);

         void InitializeCanvasInputSystem(const std::tuple<int32_t /*lua proxy id*/> &data);

         void AddCanvasFadeAnimation(const std::tuple<int32_t /*lua proxy id*/, int32_t /*0 - fadeIn, 1 - fadeOut*/, std::string /*animation json data*/> &data);

         void EnableMouseInputReceiverBase(const std::tuple<int32_t /*lua proxy id*/> &data);

         void EnableToggleButtonMouseInputReceiver(const std::tuple<int32_t /*lua proxy id*/> &data);

         void SetUiWidgetParent(const std::tuple<int32_t /*lua proxy id*/, std::string /*canvas name*/, std::string /*parent name*/> &data);

         void AddUiItemAnimation(const std::tuple<int32_t /*lua proxy id*/, std::string /*animation json data*/> &data);

         void StartUiItemAnimation(const std::tuple<int32_t /*lua proxy id*/, std::string /*animation name*/>& data);
      };
   }
}
