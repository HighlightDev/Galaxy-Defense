#include "SceneProxyBase.h"

namespace Graphics
{
   int32_t SceneProxyBase::SceneProxyIdCounter = 0;

   SceneProxyBase::SceneProxyBase(const bool isEnabled)
       : mSceneProxyId(SceneProxyIdCounter),
         mGameObjectId(),
         mIsEnabled(isEnabled)
   {
      SceneProxyIdCounter++;
   }

   SceneProxyBase::~SceneProxyBase()
   {
   }

   int32_t SceneProxyBase::GetSceneProxyId() const
   {
      return mSceneProxyId;
   }

   bool SceneProxyBase::IsEnabled() const
   {
      return mIsEnabled;
   }

   void SceneProxyBase::SetEnabled(const bool bEnabled)
   {
      mIsEnabled = bEnabled;
   }

   void SceneProxyBase::SetBindedGameObjectId(const int32_t gameObjectId)
   {
      mGameObjectId = gameObjectId;
   }

   int32_t SceneProxyBase::GetGameObjectId() const
   {
      return mGameObjectId;
   }
}
