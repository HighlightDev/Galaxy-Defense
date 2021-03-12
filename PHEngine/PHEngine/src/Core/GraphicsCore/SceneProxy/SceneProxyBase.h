#pragma once

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

      bool mIsVisible;

      bool mIsEnabled;

   public:

      SceneProxyBase(bool bVisible);

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

      void SetVisibility(const bool visibility);

      void SetEnabled(const bool bEnabled);

      bool IsVisible() const;

      bool IsEnabled() const;

   };
}

