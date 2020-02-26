#pragma once

#include "Core/CommonCore/CommonMeta.h"

#include <unordered_map>
#include <stdint.h>
#include <algorithm>
#include <memory>

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
			static sharedValue_t Allocation(key_t& key)
			{
				return policy_t::template AllocateMemory<T>(key);
			}
		};

		template <>
		struct DoIfHasInnerType<NullType>
		{
			static sharedValue_t Allocation(key_t& key)
			{
				return policy_t::AllocateMemory(key);
			}
		};

		//META DATA

	protected:

		std::unordered_map<key_t, sharedValue_t> resourceMap;
		std::unordered_map<key_t, int32_t> referenceMap;

	private:

		void IncreaseRefCounter(key_t& key)
		{
			auto it = referenceMap.find(key);
			if (it != referenceMap.end())
			{
				++it->second;
			}
			else
			{
				referenceMap.insert(std::make_pair(key, 1));
			}
		}

		sharedValue_t GetResource(key_t& key) const
		{
			sharedValue_t value;
			auto it = resourceMap.find(key);
			if (it != resourceMap.end())
			{
				value = it->second;
			}

			return value;
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

		void FreeResource(key_t& key)
		{
			auto it = referenceMap.find(key);
			if (it != referenceMap.end())
			{
				--it->second;
				if (it->second == 0)
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
      sharedValue_t GetOrAllocateResourceBridge(key_t& key)
      {
         sharedValue_t resource = GetResource(key);
         if (!resource)
         {
            resource = DoIfHasInnerType<InnerAllocationType>::Allocation(key);
            std::pair<key_t, sharedValue_t> pair = std::make_pair(key, resource);
            resourceMap.emplace(std::move(pair));
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
      sharedValue_t GetOrAllocateResource(const key_t& key)
      {
         key_t localKey = key;
         return GetOrAllocateResourceBridge<InnerAllocationType>(localKey);
      }

		template <typename InnerAllocationType = NullType>
		sharedValue_t GetOrAllocateResource(key_t& key)
		{
         return GetOrAllocateResourceBridge<InnerAllocationType>(key);
		}

		int32_t GetReferenceCount(key_t& key) const
		{
			int32_t referenceCount = 0;
			auto it = referenceMap.find(key);
			if (it != referenceMap.end())
				referenceCount = it->second;

			return referenceCount;
		}

		size_t GetResourcesCount() const
		{
			size_t resourceCount = resourceMap.size();

			return resourceCount;
		}

		bool TryToFreeMemory(key_t& key)
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

