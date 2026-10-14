#pragma once

#include "Core/CommonCore/Assertion.h"
#include "StateProperty.h"
#include "StateTransition.h"

#include <map>
#include <memory>
#include <vector>

namespace EngineCore {

class State {
    std::string mStateName;

    std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> mStateProperties;

    std::map<std::string /*dstStateName*/, StateTransition> mTransitions;

public:
    State(const std::string& stateName)
        : mStateName(stateName)
        , mStateProperties()
    {
    }

    ~State()
    {
    }

    std::string GetStateName() const
    {
        return mStateName;
    }

    void AddStateProperty(const std::shared_ptr<BaseStateProperty>& stateProperty)
    {
        auto bindingSP = stateProperty->Binding.lock();
        ext_assert(bindingSP, "State property binding is null in State::AddStateProperty");
        const std::string& name = bindingSP->BindingName;
        ext_assert(
            mStateProperties.count(name) == 0,
            "State property already exists in State::AddStateProperty"); // make sure that property doesn't duplicate
        mStateProperties.emplace(std::make_pair(name, stateProperty));
    }

    void AddStateTransition(const StateTransition& dstStateTransition)
    {
        if (auto spDestination = dstStateTransition.StateDestination.lock()) {
            const std::string& dstStateName = spDestination->GetStateName();

            ext_assert(
                mTransitions.count(dstStateName) == 0,
                "State transition already exists in State::AddStateTransition"); // make sure that transition doesn't duplicate

            mTransitions.emplace(std::make_pair(dstStateName, dstStateTransition));
        }
    }

    const std::map<std::string /*dstStateName*/, StateTransition>& GetTransitions() const
    {
        return mTransitions;
    }

    std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> GetStateProperties()
    {
        return mStateProperties;
    }
};

} // namespace EngineCore
