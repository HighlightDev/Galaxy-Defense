#include "SceneProxyBase.h"

namespace Graphics 
{
   size_t SceneProxyBase::SceneProxyIdCounter = 0;

   SceneProxyBase::SceneProxyBase()
      : mSceneProxyId(SceneProxyIdCounter)
      , mIsVisible(true)
   {
      SceneProxyIdCounter++;
   }

   SceneProxyBase::~SceneProxyBase()
   {
   }

   size_t SceneProxyBase::GetSceneProxyId() const {

      return mSceneProxyId;
   }

   bool SceneProxyBase::IsVisible() const
   {
      return mIsVisible;
   }

   void SceneProxyBase::SetVisibility(const bool visibility)
   {
      mIsVisible = visibility;
   }
}
