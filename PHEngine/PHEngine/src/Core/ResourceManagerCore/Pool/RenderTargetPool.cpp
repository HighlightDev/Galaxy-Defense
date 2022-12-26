#include "RenderTargetPool.h"
#include "Core/CommonCore/Assertion.h"

namespace Resources
{
   std::shared_ptr<RenderTargetPool> RenderTargetPool::m_instance;

   std::shared_ptr<ITexture> RenderTargetPool::GetTextureAt(const size_t index) const
   {
      assert(index < resourceMap.size());
      resourceMap_t::const_iterator startIt = resourceMap.begin();
      std::advance(startIt, index);
      return startIt->second;
   }

   size_t RenderTargetPool::GetTexturesCount() const
   {
      return GetResourcesCount();
   }

   std::string RenderTargetPool::ToString() const
   {
      return "RenderTargetPool";
   }

   std::shared_ptr<RenderTargetPool> RenderTargetPool::GetInstance()
   {
      if (!m_instance)
         m_instance = std::make_shared<RenderTargetPool>();

      return m_instance;
   }

   void RenderTargetPool::ReloadInstance()
   {
      if (m_instance)
         m_instance.reset();
   }
}
