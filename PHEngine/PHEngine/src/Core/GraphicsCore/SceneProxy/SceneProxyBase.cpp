#include "SceneProxyBase.h"

namespace Graphics 
{
   size_t SceneProxyBase::SceneProxyIdCounter = 0;

   SceneProxyBase::SceneProxyBase(const bool isEnabled)
      : mSceneProxyId(SceneProxyIdCounter)
      , mIsEnabled(isEnabled)
   {
      SceneProxyIdCounter++;
   }

   SceneProxyBase::~SceneProxyBase()
   {
   }

   size_t SceneProxyBase::GetSceneProxyId() const {

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
}
