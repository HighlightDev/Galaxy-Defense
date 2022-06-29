#include "RenderTargetPool.h"

namespace Resources
{

   std::unique_ptr<RenderTargetPool> RenderTargetPool::m_instance;

   std::shared_ptr<ITexture> RenderTargetPool::GetRenderTargetAt(size_t index) const
   {
      std::shared_ptr<ITexture> result(nullptr);
      if (index < resourceMap.size())
      {
         resourceMap_t::const_iterator startIt = resourceMap.begin();
         std::advance(startIt, index);
         result = startIt->second;
      }

      return result;
   }

   std::string RenderTargetPool::ToString() const
   {
      return "RenderTargetPool";
   }

   std::unique_ptr<RenderTargetPool> &RenderTargetPool::GetInstance()
   {

      if (!m_instance)
         m_instance = std::make_unique<RenderTargetPool>();

      return m_instance;
   }

   void RenderTargetPool::ReloadInstance()
   {
      if (m_instance)
         m_instance.reset();
   }
}
