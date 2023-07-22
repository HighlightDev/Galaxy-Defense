#pragma once

#include <cstddef>
#include <stdint.h>

namespace Graphics {

   class SceneProxyBase
   {
      static int32_t SceneProxyIdCounter;

   protected:

      int32_t mSceneProxyId;

      int32_t mGameObjectId;

      bool mIsEnabled;

   public:

      SceneProxyBase(const bool isEnabled);

      virtual ~SceneProxyBase();

      int32_t GetSceneProxyId() const;

      void SetEnabled(const bool bEnabled);

      bool IsEnabled() const;

      void SetBindedGameObjectId(const int32_t gameObjectId);

      int32_t GetGameObjectId() const;

   };
}

