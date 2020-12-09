#pragma once

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

      bool mIsVisible;

   public:

      SceneProxyBase();

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

      void SetVisibility(const bool visibility);

      bool IsVisible() const;

   };
}

