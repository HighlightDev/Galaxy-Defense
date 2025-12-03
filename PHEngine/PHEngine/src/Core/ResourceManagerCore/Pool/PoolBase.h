#pragma once

#include "Core/CommonCore/CommonMeta.h"
#include "Core/GameCore/LoggerExtension.h"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>

using namespace EngineCore;

namespace Resources {

template<typename AllocationType, typename ValueType>
concept Deallocatable = requires(std::shared_ptr<ValueType> v)
{
    AllocationType::DeallocateMemory(v);
};

template<typename AllocationType, typename KeyType, typename ValueType>
concept Allocatable = requires(const KeyType& k)
{
    {
        AllocationType::AllocateMemory(k)
    }
    ->std::same_as<std::shared_ptr<ValueType>>;
};

template<typename ValueType, typename KeyType, typename AllocationPolicyType>
requires Deallocatable<AllocationPolicyType, ValueType>&& Allocatable<AllocationPolicyType, KeyType, ValueType> class PoolBase {
public:
    using value_t = ValueType;
    using key_t = KeyType;
    using allocationPolicy_t = AllocationPolicyType;
    using sharedValue_t = std::shared_ptr<ValueType>;
    using resourceMap_t = std::unordered_map<key_t, sharedValue_t>;

protected:
    std::unordered_map<key_t, sharedValue_t> resourceMap;
    std::unordered_map<key_t, int32_t> referenceMap;

    std::function<void()> mOnDisposeCallback;

private:
    void IncreaseRefCounter(const key_t& key)
    {
        if (referenceMap.count(key)) {
            ++referenceMap[key];
        } else {
            referenceMap[key] = 1;
        }
    }

    sharedValue_t GetResource(const key_t& key) const
    {
        sharedValue_t value;
        if (resourceMap.count(key)) {
            value = resourceMap.at(key);
        }

        return value;
    }

    void FreeResource(const key_t& key)
    {
        if (referenceMap.count(key)) {
            auto& referenceCount = referenceMap[key];
            --referenceCount;
            if (referenceCount == 0) {
                allocationPolicy_t::DeallocateMemory(resourceMap[key]);
                resourceMap.erase(key);
                referenceMap.erase(key);
            }
        }

        if (referenceMap.size() <= 0) {
            if (mOnDisposeCallback) {
                mOnDisposeCallback();
            }
        }
    }

    template<typename InnerAllocationType, typename Key_t>
    typename std::enable_if<!std::is_same<InnerAllocationType, Common::NullType>::value, sharedValue_t>::type
    GetOrAllocateResourceBridge(const Key_t& key)
    {
        sharedValue_t resource = GetResource(key);
        if (!resource) {
            resource = allocationPolicy_t::template AllocateMemory<InnerAllocationType>(key);
            resourceMap.emplace(key, resource);
        }

        if (resource) {
            IncreaseRefCounter(key);
        }

        return resource;
    }

    template<typename InnerAllocationType, typename Key_t>
    typename std::enable_if<std::is_same<InnerAllocationType, Common::NullType>::value, sharedValue_t>::type
    GetOrAllocateResourceBridge(const Key_t& key)
    {
        sharedValue_t resource = GetResource(key);
        if (!resource) {
            resource = allocationPolicy_t::AllocateMemory(key);
            resourceMap.emplace(key, resource);
        }

        if (resource) {
            IncreaseRefCounter(key);
        }

        return resource;
    }

public:
    PoolBase(std::function<void()> onDisposeCallback = nullptr)
        : mOnDisposeCallback(onDisposeCallback)
    {
    }

    virtual ~PoolBase() = default;

    virtual std::string ToString() const = 0;

    void CleanUp()
    {
        LogInfo(ToString(), "::CleanUp");
        for (auto it = resourceMap.begin(); it != resourceMap.end(); ++it) {
            auto key = it->first;
            allocationPolicy_t::DeallocateMemory(resourceMap[key]);
        }

        resourceMap.clear();
        referenceMap.clear();

        if (mOnDisposeCallback) {
            mOnDisposeCallback();
        }
    }

    template<typename InnerAllocationType = Common::NullType, typename Key_t>
    typename std::enable_if<!std::is_same<InnerAllocationType, Common::NullType>::value, std::shared_ptr<InnerAllocationType>>::
        type
        GetOrAllocateResource(const Key_t& key)
    {
        return std::static_pointer_cast<InnerAllocationType>(GetOrAllocateResourceBridge<InnerAllocationType>(key));
    }

    template<typename InnerAllocationType = Common::NullType, typename Key_t>
    typename std::enable_if<std::is_same<InnerAllocationType, Common::NullType>::value, sharedValue_t>::type
    GetOrAllocateResource(const Key_t& key)
    {
        return GetOrAllocateResourceBridge<InnerAllocationType>(key);
    }

    std::optional<key_t> GetKeyOptional(const sharedValue_t& value) const
    {
        auto predicate = [&value](auto& keyvalue) {
            if (keyvalue.second == value)
                return true;
            if (keyvalue.second && value)
                return (*keyvalue.second) == (*value);
            return false;
        };
        auto it = std::find_if(resourceMap.begin(), resourceMap.end(), predicate);

        if (it != resourceMap.end())
            return it->first;

        return std::nullopt;
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

        if (resource) {
            bMemoryFreed = true;
            FreeResource(key);
        }

        return bMemoryFreed;
    }

    bool TryToFreeMemory(sharedValue_t value)
    {
        bool bMemoryFreed = false;
        const std::optional<key_t>& optionalKey = GetKeyOptional(value);

        if (optionalKey.has_value()) {
            bMemoryFreed = true;
            FreeResource(*optionalKey);
        }

        return bMemoryFreed;
    }

    std::vector<key_t> GetAllKeys() const
    {
        std::vector<key_t> keys;
        for (const auto& resourcePair : resourceMap) {
            keys.push_back(resourcePair.first);
        }
        return keys;
    }
};

} // namespace Resources
