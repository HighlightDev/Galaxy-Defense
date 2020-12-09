#pragma once

namespace Graphics {

   class SceneProxyBase
   {
      static size_t SceneProxyIdCounter;

   protected:

      size_t mSceneProxyId;

   public:

      SceneProxyBase();

      virtual ~SceneProxyBase();

      size_t GetSceneProxyId() const;

   };
}

