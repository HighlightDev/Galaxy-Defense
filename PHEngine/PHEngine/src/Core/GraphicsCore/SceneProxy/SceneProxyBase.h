#pragma once

#include <cstddef>

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

      bool mIsEnabled;

   public:

      SceneProxyBase(const bool isEnabled);

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

      void SetEnabled(const bool bEnabled);

      bool IsEnabled() const;

   };
}

