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
#include <vector>

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

template<typename ValueType>
concept Comparable = requires(const ValueType& a, const ValueType& b)
{
    {
        a == b
    }
    ->std::convertible_to<bool>;
};

template<typename ValueType, typename KeyType, typename AllocationPolicyType>
requires Deallocatable<AllocationPolicyType, ValueType>&& Allocatable<AllocationPolicyType, KeyType, ValueType>&&
    Comparable<ValueType> class PoolBase {
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
        ++referenceMap[key];
    }

    sharedValue_t GetResource(const key_t& key) const
    {
        auto it = resourceMap.find(key);
        if (it != resourceMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void FreeResource(const key_t& key)
    {
        auto refIt = referenceMap.find(key);
        if (refIt != referenceMap.end()) {
            if (--refIt->second <= 0) {
                resourceMap.erase(key);
                referenceMap.erase(refIt);
                EngineCore::LogInfo(ToString(), "::FreeResource - Freed resource with key: ", key);
            }
        }

        if (referenceMap.empty()) {
            if (mOnDisposeCallback) {
                mOnDisposeCallback();
            }
        }
    }

    static sharedValue_t WrapWithCustomDeleter(sharedValue_t allocated)
    {
        return sharedValue_t(allocated.get(), [allocated](ValueType*) mutable {
            allocationPolicy_t::DeallocateMemory(allocated);
            allocated.reset();
        });
    }

    template<typename InnerAllocationType, typename Key_t>
    sharedValue_t GetOrAllocateResourceImpl(const Key_t& key)
    {
        sharedValue_t resource = GetResource(key);
        if (!resource) {
            sharedValue_t allocated;
            if constexpr (std::is_same_v<InnerAllocationType, Common::NullType>) {
                allocated = allocationPolicy_t::AllocateMemory(key);
            } else {
                allocated = allocationPolicy_t::template AllocateMemory<InnerAllocationType>(key);
            }
            if (allocated) {
                resource = WrapWithCustomDeleter(std::move(allocated));
                resourceMap.insert_or_assign(key, resource);
            }
        }

        if (resource) {
            IncreaseRefCounter(key);
        }

        return resource;
    }

public:
    PoolBase(std::function<void()> onDisposeCallback = nullptr)
        : mOnDisposeCallback(std::move(onDisposeCallback))
    {
    }

    virtual ~PoolBase() = default;

    virtual std::string ToString() const = 0;

    void CleanUp()
    {
        EngineCore::LogInfo(ToString(), "::CleanUp");

        for (const auto& [key, resource] : resourceMap) {
            if (resource) {
                allocationPolicy_t::DeallocateMemory(resource);
            }
        }

        resourceMap.clear();
        referenceMap.clear();

        if (mOnDisposeCallback) {
            mOnDisposeCallback();
        }
    }

    template<typename InnerAllocationType = Common::NullType, typename Key_t>
    auto GetOrAllocateResource(const Key_t& key)
    {
        if constexpr (!std::is_same_v<InnerAllocationType, Common::NullType>) {
            return std::static_pointer_cast<InnerAllocationType>(GetOrAllocateResourceImpl<InnerAllocationType>(key));
        } else {
            return GetOrAllocateResourceImpl<InnerAllocationType>(key);
        }
    }

    std::optional<key_t> GetKeyOptional(const sharedValue_t& value) const
    {
        auto predicate = [&value](const auto& keyvalue) {
            const auto& resource = keyvalue.second;
            if (resource == value)
                return true;
            if (resource && value)
                return (*resource) == (*value);
            return false;
        };
        auto it = std::find_if(resourceMap.begin(), resourceMap.end(), predicate);

        if (it != resourceMap.end())
            return it->first;

        return std::nullopt;
    }

    int32_t GetReferenceCount(const key_t& key) const
    {
        auto it = referenceMap.find(key);
        if (it != referenceMap.end())
            return it->second;

        return 0;
    }

    size_t GetResourcesCount() const
    {
        return resourceMap.size();
    }

    bool TryToFreeMemory(const key_t& key)
    {
        sharedValue_t resource = GetResource(key);
        if (resource) {
            FreeResource(key);
            return true;
        }
        return false;
    }

    bool TryToFreeMemory(const sharedValue_t& value)
    {
        auto optionalKey = GetKeyOptional(value);
        if (optionalKey.has_value()) {
            FreeResource(*optionalKey);
            return true;
        }
        return false;
    }

    std::vector<key_t> GetAllKeys() const
    {
        std::vector<key_t> keys;
        keys.reserve(resourceMap.size());
        for (const auto& [key, _] : resourceMap) {
            keys.push_back(key);
        }
        return keys;
    }
};

} // namespace Resources
