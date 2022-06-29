#include "SimplePrimitivePool.h"

namespace Resources
{
   std::unique_ptr<SimplePrimitivePool> SimplePrimitivePool::m_instance;

   std::string SimplePrimitivePool::ToString() const
   {
      return "SimplePrimitivePool";
   }

   std::unique_ptr<SimplePrimitivePool> &SimplePrimitivePool::GetInstance()
   {
      if (!m_instance)
         m_instance = std::make_unique<SimplePrimitivePool>();

      return m_instance;
   }

   void SimplePrimitivePool::ReloadInstance()
   {
      if (m_instance)
         m_instance.reset();
   }
}