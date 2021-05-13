#pragma once

#include "Core/CommonCore/CommonMeta.h"

#include <unordered_map>
#include <stdint.h>
#include <algorithm>
#include <memory>
#include <type_traits>

using namespace Common;

namespace Resources
{

	template <typename ValueType, typename KeyType, template <typename> typename AllocationPolicyType>
	class PoolBase
	{
	public:

		using value_t = ValueType;
		using key_t = KeyType;
		using policy_t = AllocationPolicyType<typename key_t>;
		using sharedValue_t = std::shared_ptr<ValueType>;
      using resourceMap_t = std::unordered_map<key_t, sharedValue_t>;

	private:
		
		// META DATA

		template <typename T>
		struct DoIfHasInnerType
		{
			static sharedValue_t Allocation(const key_t& key)
			{
				return policy_t::template AllocateMemory<T>(key);
			}
		};

		template <>
		struct DoIfHasInnerType<NullType>
		{
			static sharedValue_t Allocation(const key_t& key)
			{
				return policy_t::AllocateMemory(key);
			}
		};

		//META DATA

	protected:

		std::unordered_map<key_t, sharedValue_t> resourceMap;
		std::unordered_map<key_t, int32_t> referenceMap;

	private:

		void IncreaseRefCounter(const key_t& key)
		{
         if (referenceMap.count(key))
         {
            ++referenceMap[key];
         }
			else
			{
            referenceMap[key] = 1;
			}
		}

      sharedValue_t GetResource(const key_t& key) const
      {
         sharedValue_t value;
         if (resourceMap.count(key))
         {
            value = resourceMap.at(key);
         }

         return value;
      }

		void FreeResource(const key_t& key)
		{
			if (referenceMap.count(key))
			{
            auto referenceCount = referenceMap[key];
				--referenceCount;
				if (referenceCount == 0)
				{
					policy_t::DeallocateMemory(resourceMap[key]);
					resourceMap.erase(key);
					referenceMap.erase(key);
				}
			}
		}

		void CleanUp()
		{
			for (auto it = resourceMap.begin(); it != resourceMap.end(); ++it)
			{
				 auto key = it->first;
				 policy_t::DeallocateMemory(resourceMap[key]);
			}

			resourceMap.clear();
			referenceMap.clear();
		}

      template <typename InnerAllocationType>
      sharedValue_t GetOrAllocateResourceBridge(const key_t& key)
      {
         sharedValue_t resource = GetResource(key);
         if (!resource)
         {
            resource = DoIfHasInnerType<InnerAllocationType>::Allocation(key);
            resourceMap.emplace(key, resource);
         }

         if (resource)
         {
            IncreaseRefCounter(key);
         }

         return resource;
      }

	public:

		PoolBase() {}

		virtual ~PoolBase() 
		{
			CleanUp();
		}


		template <typename InnerAllocationType = NullType>
      typename std::enable_if<!std::is_same<InnerAllocationType, NullType>::value, std::shared_ptr<InnerAllocationType>>::type GetOrAllocateResource(const key_t& key)
		{
         return std::static_pointer_cast<InnerAllocationType>(GetOrAllocateResourceBridge<InnerAllocationType>(key));
		}

      template <typename InnerAllocationType = NullType>
      typename std::enable_if<std::is_same<InnerAllocationType, NullType>::value, sharedValue_t>::type GetOrAllocateResource(const key_t& key)
      {
         return GetOrAllocateResourceBridge<InnerAllocationType>(key);
      }

      key_t GetKey(sharedValue_t value) const
      {
         key_t key;
         auto predicate = [&value](auto& keyvalue)
         {
            return (keyvalue.second == value);
         };
         auto it = std::find_if(resourceMap.begin(), resourceMap.end(), predicate);

         if (it != resourceMap.end())
            key = it->first;

         return key;
      }

		int32_t GetReferenceCount(const key_t& key) const
		{
         if (referenceMap.count(key))
            return referenceMap.at(key);

			return 0;
		}

		size_t GetResourcesCount() const
		{
			size_t resourceCount = resourceMap.size();

			return resourceCount;
		}

		bool TryToFreeMemory(const key_t& key)
		{
			bool bMemoryFreed = false;
			sharedValue_t resource = GetResource(key);

			if (resource)
			{
				bMemoryFreed = true;
				FreeResource(key);
			}

			return bMemoryFreed;
		}

		bool TryToFreeMemory(sharedValue_t value)
		{
			bool bMemoryFreed = false;
         key_t key = GetKey(value);

			if (&(key) != nullptr)
			{
				bMemoryFreed = true;
				FreeResource(key);
			}

			return bMemoryFreed;
		}
	};

}

