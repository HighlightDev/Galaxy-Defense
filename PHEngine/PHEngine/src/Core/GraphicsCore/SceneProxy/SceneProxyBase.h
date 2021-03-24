#pragma once

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

      bool mIsEnabled;

   public:

      SceneProxyBase();

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

      void SetEnabled(const bool bEnabled); //todo: make set enabled not only for primitive components but also for other components (cameras, planar refl component)

      bool IsEnabled() const;

   };
}

