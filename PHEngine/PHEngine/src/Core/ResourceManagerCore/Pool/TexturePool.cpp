#include "TexturePool.h"

namespace Resources
{

   std::unique_ptr<TexturePool> TexturePool::m_instance;

   std::shared_ptr<ITexture> TexturePool::GetTextureAt(size_t index) const
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

   std::string TexturePool::ToString() const
   {
      return "TexturePool";
   }

   std::unique_ptr<TexturePool> &TexturePool::GetInstance()
   {
      if (!m_instance)
         m_instance = std::move(std::make_unique<TexturePool>());

      return m_instance;
   }

   void TexturePool::ReloadInstance()
   {
      if (m_instance)
         m_instance.reset();
   }
}