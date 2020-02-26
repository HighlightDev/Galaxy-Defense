#include "RenderTargetPool.h"

namespace Resources
{

   std::unique_ptr<RenderTargetPool> RenderTargetPool::m_instance;

   std::shared_ptr<ITexture> RenderTargetPool::GetRenderTargetAt(size_t index) const {
      std::shared_ptr<ITexture> result(nullptr);
      if (index < resourceMap.size())
      {
         resourceMap_t::const_iterator startIt = resourceMap.begin();
         std::advance(startIt, index);
         result = startIt->second;
      }

      return result;
   }
}
