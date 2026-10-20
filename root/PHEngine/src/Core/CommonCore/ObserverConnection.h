#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace EngineCore {

template<typename T>
class ObserverConnection;

template<typename... ArgsType>
class ObserverConnection<void(ArgsType...)> : public std::enable_shared_from_this<ObserverConnection<void(ArgsType...)>> {

    struct PrivateTag {
        explicit PrivateTag() = default;
    };

public:
    using type_t = ObserverConnection<void(ArgsType...)>;

    explicit ObserverConnection(PrivateTag)
    {
    }

    ObserverConnection(const ObserverConnection&) = delete;
    ObserverConnection& operator=(const ObserverConnection&) = delete;

    static std::shared_ptr<type_t> CreateInstance()
    {
        return std::make_shared<type_t>(PrivateTag{});
    }

    class SubscriptionHandler {
        static inline int32_t s_subscriptionId;
        int32_t mSubscriptionId{0};
        std::weak_ptr<type_t> mParentWp;

    public:
        explicit SubscriptionHandler(std::weak_ptr<type_t> parentWp)
            : mSubscriptionId(s_subscriptionId++)
            , mParentWp(parentWp)
        {
        }

        ~SubscriptionHandler()
        {
            (void)Unsubscribe();
        }

        SubscriptionHandler(const SubscriptionHandler&) = delete;
        SubscriptionHandler& operator=(const SubscriptionHandler&) = delete;
        SubscriptionHandler(SubscriptionHandler&& rh) = delete;
        SubscriptionHandler& operator=(SubscriptionHandler&& rh) = delete;

        int32_t GetSubscriptionId() const
        {
            return mSubscriptionId;
        }

        bool Unsubscribe()
        {
            if (const auto parentSp = mParentWp.lock()) {
                return parentSp->Unsubscribe(mSubscriptionId);
            }
            return false;
        }
    };

    bool Unsubscribe(const int32_t subscriptionId)
    {
        {
            std::lock_guard<std::mutex> lock(mSyncMutex);
            auto foundSubIt = std::find_if(mSubscribers.begin(), mSubscribers.end(), [subscriptionId](const auto& subPair) {
                if (const auto subscriptionSp = subPair.second.lock()) {
                    return subscriptionId == subscriptionSp->GetSubscriptionId();
                }
                return false;
            });
            if (foundSubIt != mSubscribers.end()) {
                mSubscribers.erase(foundSubIt);
                return true;
            }

            PurifySubscriptions();
        }
        return false;
    }

    [[nodiscard]] std::shared_ptr<SubscriptionHandler> Subscribe(std::function<void(ArgsType...)> functor)
    {
        auto handlerSp = std::make_shared<SubscriptionHandler>(type_t::weak_from_this());
        {
            std::lock_guard<std::mutex> lock(mSyncMutex);
            mSubscribers.emplace_back(std::move(functor), handlerSp);
        }
        return handlerSp;
    }

    void NotifyAll(const ArgsType&... args)
    {
        const auto selfSp = this->shared_from_this(); // holding strong ref of myself untill the end of notification
        {
            std::vector<std::pair<std::function<void(ArgsType...)>, std::weak_ptr<SubscriptionHandler>>> _subscribers;
            {
                std::lock_guard<std::mutex> lock(mSyncMutex);
                _subscribers = mSubscribers; // Prevents broken iterator in case if unsubscribe happens inside callback
            }

            for (const auto& [callback, subscription] : _subscribers) {
                if (const auto& subscriptionSp = subscription.lock()) {
                    const bool isStillActiveSubscription
                        = std::any_of(mSubscribers.cbegin(), mSubscribers.cend(), [subscriptionSp](const auto& ownedSubscriber) {
                              return subscriptionSp == ownedSubscriber.second.lock();
                          });

                    // Don't invoke callback if subscription was terminated in previous callbacks
                    if (isStillActiveSubscription) {
                        std::invoke(callback, args...);
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(mSyncMutex);
                PurifySubscriptions();
            }
        }
    }

private:
    void PurifySubscriptions()
    {
        // Remove dead subscriptions
        mSubscribers.erase(
            std::remove_if(
                mSubscribers.begin(), mSubscribers.end(), [](const auto& subPair) { return subPair.second.expired(); }),
            mSubscribers.end());
    }

    std::mutex mSyncMutex;

    std::vector<std::pair<std::function<void(ArgsType...)>, std::weak_ptr<SubscriptionHandler>>> mSubscribers;
};
} // namespace EngineCore
