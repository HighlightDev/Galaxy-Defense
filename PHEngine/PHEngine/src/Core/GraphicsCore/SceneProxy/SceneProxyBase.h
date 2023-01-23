#pragma once

#include <cstddef>

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

      size_t mGameObjectId;

      bool mIsEnabled;

   public:

      SceneProxyBase(const bool isEnabled);

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

      void SetEnabled(const bool bEnabled);

      bool IsEnabled() const;

      void SetBindedGameObjectId(const size_t gameObjectId);

      size_t GetGameObjectId() const;

   };
}

