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

         void SetScene(const std::weak_ptr<Scene> &sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) override;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

      private:
         std::string GetCurrentOverlayName(const std::tuple<> &data);

         void OpenOverlay(const std::tuple<std::string> &overlayName);

         void CloseCurrentOverlay(const std::tuple<> &emptyData);

         int32_t CreateCommonUiWidget(const std::tuple<int32_t, std::string> &data);

         bool IsLuaProxyReady(const std::tuple<int32_t>& data);

         std::string GetUiWidgetName(const std::tuple<int32_t>& data);

         void OnCommonUiWidgetDataUpdated(const std::tuple<int32_t/*lua proxy id*/, std::string/*json data*/>& data);

         std::string GetGameThreadData(const std::tuple<int32_t/*lua proxy id*/>& data);

         void InitializeCanvasInputSystem(const std::tuple<int32_t/*lua proxy id*/>& data);
      };
   }
}
