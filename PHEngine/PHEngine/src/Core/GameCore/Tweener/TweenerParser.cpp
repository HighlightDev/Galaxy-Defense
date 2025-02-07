#include "TweenerParser.h"

#include "Core/CommonCore/XMLParserHelper.h"
#include "Core/GameCore/EngineObjectPropertyBindings/BooleanPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/EulerAnglesRotationPropertyBinding.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec3PropertyBinding.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <type_traits>
#include <unordered_map>

using namespace Common;
using namespace EngineUtility;

namespace EngineCore {
#define TWEENER_START_NODE_NAME "<tweener>"
#define TWEENER_END_NODE_NAME "</tweener>"
#define TWEENER_NAME_START_NODE "<tweener_name>"
#define TWEENER_NAME_END_NODE "</tweener_name>"
#define STATES_START_NODE_NAME "<states>"
#define STATES_END_NODE_NAME "</states>"
#define STATE_START_NODE_NAME "<state>"
#define STATE_END_NODE_NAME "</state>"
#define TRANSITIONS_START_NODE_NAME "<transitions>"
#define TRANSITIONS_END_NODE_NAME "</transitions>"
#define TRANSITION_START_NODE_NAME "<transition>"
#define TRANSITION_END_NODE_NAME "</transition>"
#define BINDINGS_START_NODE_NAME "<bindings>"
#define BINDINGS_END_NODE_NAME "</bindings>"
#define BINDING_START_NODE_NAME "<binding>"
#define BINDING_END_NODE_NAME "</binding>"
#define PROPERTIES_START_NODE_NAME "<properties>"
#define PROPERTIES_END_NODE_NAME "</properties>"
#define PROPERTY_START_NODE_NAME "<property>"
#define PROPERTY_END_NODE_NAME "</property>"

TweenerParser::TweenerParser_Property
GetPropertyAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
{
    auto propertyStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, PROPERTY_START_NODE_NAME);
    auto propertyEndNode = XMLParserHelper::GetItByNodeName(propertyStartNode, endIt, PROPERTY_END_NODE_NAME);
    ++propertyStartNode;

    TweenerParser::TweenerParser_Property property;

    for (auto it = propertyStartNode; it != propertyEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        std::string name, from, to, duration;

        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            property.Name = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "binding_name")) {
            property.BindingName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "type")) {
            property.Type = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "value")) {
            property.Value = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "state")) {
            property.State = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else {
            assert(false);
        }
    }

    beginIt = propertyEndNode;
    return property;
}

TweenerParser::TweenerParser_Binding
GetBindingAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
{
    auto bindingStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, BINDING_START_NODE_NAME);
    auto bindingEndNode = XMLParserHelper::GetItByNodeName(bindingStartNode, endIt, BINDING_END_NODE_NAME);
    ++bindingStartNode;

    TweenerParser::TweenerParser_Binding binding;

    for (auto it = bindingStartNode; it != bindingEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        std::string name, from, to, duration;

        if (EngineUtility::StartsWith(currentNodeStr, "binding_name")) {
            binding.BindingName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "type")) {
            binding.Type = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else {
            assert(false);
        }
    }

    beginIt = bindingEndNode;
    return binding;
}

TweenerParser::TweenerParser_Transition
GetTransitionAndAdvanceIt(XMLParserHelper::iterator_t& beginIt, const XMLParserHelper::iterator_t& endIt)
{
    auto transitionStartNode = XMLParserHelper::GetItByNodeName(beginIt, endIt, TRANSITION_START_NODE_NAME);
    auto transitionEndNode = XMLParserHelper::GetItByNodeName(transitionStartNode, endIt, TRANSITION_END_NODE_NAME);
    ++transitionStartNode;

    TweenerParser::TweenerParser_Transition transition;

    for (auto it = transitionStartNode; it != transitionEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        std::string name, from, to, duration;

        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            transition.Name = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "from_state")) {
            transition.From = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "to_state")) {
            transition.To = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "duration")) {
            transition.Duration = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else {
            assert(false);
        }
    }

    beginIt = transitionEndNode;
    return transition;
}

template<typename T>
typename std::enable_if<std::is_same<float, T>::value, T>::type GetTrivialValueAfterAssignOperator(const std::string& src)
{
    const auto& valueStr = src.substr(IndexOf(src, "=") + 1);
    return std::stof(valueStr);
}

std::shared_ptr<Tweener> TweenerParser::ParseTweenerDescriptor(const std::string& tweenerName)
{
    const std::string& absolutePath = IO::FolderManager::GetInstance()->GetTweenerPath() + tweenerName;

    FileFacade fileWorker;
    fileWorker.OpenAndReadFile(absolutePath);
    const size_t sizeOfSrc = fileWorker.GetFileSourceLinesCount();
    assert(sizeOfSrc > 0);

    std::list<std::string> fileSource = fileWorker.GetFileSrc();
    std::string tweenerInnerName = "";

    // Collect tweener data
    {

        auto tweenStartNode = XMLParserHelper::GetItByNodeName(fileSource, TWEENER_START_NODE_NAME);
        auto tweenEndNode = XMLParserHelper::GetItByNodeName(fileSource, TWEENER_END_NODE_NAME);
        ++tweenStartNode;

        auto tweenStartName = XMLParserHelper::GetItByNodeName(fileSource, TWEENER_NAME_START_NODE);
        auto tweenEndName = XMLParserHelper::GetItByNodeName(fileSource, TWEENER_NAME_END_NODE);

        for (auto it = tweenStartName; it != tweenEndName; ++it) {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

            if (EngineUtility::StartsWith(currentNodeStr, "name")) {
                const std::string& name = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
                tweenerInnerName = name;
            }
        }
        assert(tweenerInnerName != "");

        auto statesStartNode = XMLParserHelper::GetItByNodeName(tweenStartNode, tweenEndNode, STATES_START_NODE_NAME);
        auto statesEndNode = XMLParserHelper::GetItByNodeName(tweenStartNode, tweenEndNode, STATES_END_NODE_NAME);
        ++statesStartNode;

        for (auto it = statesStartNode; it != statesEndNode; ++it) {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

            if (EngineUtility::StartsWith(currentNodeStr, "name")) {
                const std::string& name = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
                mStates.emplace_back(TweenerParser_State(name));
            }
        }

        auto transitionsStartNode = XMLParserHelper::GetItByNodeName(statesEndNode, tweenEndNode, TRANSITIONS_START_NODE_NAME);
        auto transitionsEndNode = XMLParserHelper::GetItByNodeName(transitionsStartNode, tweenEndNode, TRANSITIONS_END_NODE_NAME);
        ++transitionsStartNode;

        for (auto it = transitionsStartNode; it != transitionsEndNode; ++it) {
            mTransitions.emplace_back(GetTransitionAndAdvanceIt(it, transitionsEndNode));
        }

        auto bindingsStartNode = XMLParserHelper::GetItByNodeName(transitionsEndNode, tweenEndNode, BINDINGS_START_NODE_NAME);
        auto bindingsEndNode = XMLParserHelper::GetItByNodeName(bindingsStartNode, tweenEndNode, BINDINGS_END_NODE_NAME);
        ++bindingsStartNode;

        for (auto it = bindingsStartNode; it != bindingsEndNode; ++it) {
            mBindings.emplace_back(GetBindingAndAdvanceIt(it, bindingsEndNode));
        }

        auto propertiesStartNode = XMLParserHelper::GetItByNodeName(bindingsEndNode, tweenEndNode, PROPERTIES_START_NODE_NAME);
        auto propertiesEndNode = XMLParserHelper::GetItByNodeName(propertiesStartNode, tweenEndNode, PROPERTIES_END_NODE_NAME);
        ++propertiesStartNode;

        for (auto it = propertiesStartNode; it != propertiesEndNode; ++it) {
            mProperties.emplace_back(GetPropertyAndAdvanceIt(it, propertiesEndNode));
        }
    }

    return BuildTweener(tweenerName, tweenerInnerName);
}

std::shared_ptr<PropertyBinding> CreatePropertyBinding(const TweenerParser::TweenerParser_Binding& binding)
{
    std::shared_ptr<PropertyBinding> result;

    if ("animation" == binding.Type) {
        result = std::make_shared<AnimationPropertyBinding>(binding.BindingName);
    } else if ("scalar_float" == binding.Type) {
        result = std::make_shared<FloatPropertyBinding>(binding.BindingName);
    } else if ("euler_angles_rotation" == binding.Type) {
        result = std::make_shared<EulerAnglesRotationPropertyBinding>(binding.BindingName);
    } else if ("boolean" == binding.Type) {
        result = std::make_shared<BooleanPropertyBinding>(binding.BindingName);
    } else if ("vec3" == binding.Type) {
        result = std::make_shared<Vec3PropertyBinding>(binding.BindingName);
    } else {
        assert(false);
    }

    return result;
}

glm::vec3 ExtractVec3FromStrings(const std::vector<std::string>& valuesStr)
{
    glm::vec3 result(0.0f);
    bool b_xValueFound = false, b_yValueFound = false, b_zValueFound = false;
    for (const auto& value : valuesStr) {
        const auto trimmedValueStr = TrimEnd(TrimStart(RemoveAll(value, ' ')));

        if (StartsWith(trimmedValueStr, "x=")) {
            const auto& valueStr = trimmedValueStr.substr(IndexOf(trimmedValueStr, "=") + 1);
            result.x = GetTrivialValueAfterAssignOperator<float>(valueStr);
            b_xValueFound = true;
        } else if (StartsWith(trimmedValueStr, "y=")) {
            const auto& valueStr = trimmedValueStr.substr(IndexOf(trimmedValueStr, "=") + 1);
            result.y = GetTrivialValueAfterAssignOperator<float>(valueStr);
            b_yValueFound = true;
        } else if (StartsWith(trimmedValueStr, "z=")) {
            const auto& valueStr = trimmedValueStr.substr(IndexOf(trimmedValueStr, "=") + 1);
            result.z = GetTrivialValueAfterAssignOperator<float>(valueStr);
            b_zValueFound = true;
        } else {
            assert(false);
        }
    }

    assert(b_xValueFound && b_yValueFound && b_zValueFound);

    return result;
}

std::unique_ptr<BaseStateProperty> CreateProperty(
    const TweenerParser::TweenerParser_Property& property,
    const std::unordered_map<std::string, std::shared_ptr<PropertyBinding>>& bindings)
{
    std::unique_ptr<BaseStateProperty> result;

    assert(bindings.count(property.BindingName));

    if ("animation" == property.Type) {
        result = std::make_unique<StateProperty<eEnginePropertyBindingType::Animation>>(
            property.Value, std::static_pointer_cast<AnimationPropertyBinding>(bindings.at(property.BindingName)));
    } else if ("scalar_float" == property.Type) {
        const float value = std::stof(property.Value);
        result = std::make_unique<StateProperty<eEnginePropertyBindingType::FloatScalar>>(
            value, std::static_pointer_cast<FloatPropertyBinding>(bindings.at(property.BindingName)));
    } else if ("euler_angles_rotation" == property.Type) {
        const auto& values = Split(property.Value, ';');
        const glm::vec3 eulerAngles = ExtractVec3FromStrings(values);
        result = std::make_unique<StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>>(
            eulerAngles, std::static_pointer_cast<EulerAnglesRotationPropertyBinding>(bindings.at(property.BindingName)));
    } else if ("boolean" == property.Type) {
        const std::string& value = ToLower(property.Value);
        const bool booleanValue = "true" == value;
        result = std::make_unique<StateProperty<eEnginePropertyBindingType::Boolean>>(
            booleanValue, std::static_pointer_cast<BooleanPropertyBinding>(bindings.at(property.BindingName)));
    } else if ("vec3" == property.Type) {
        const auto& values = Split(property.Value, ';');
        const glm::vec3 vec3Value = ExtractVec3FromStrings(values);
        result = std::make_unique<StateProperty<eEnginePropertyBindingType::Vec3>>(
            vec3Value, std::static_pointer_cast<Vec3PropertyBinding>(bindings.at(property.BindingName)));
    }

    assert(result);

    return result;
}

std::shared_ptr<Tweener> TweenerParser::BuildTweener(const std::string& relPathTweener, const std::string& tweenerInnerName)
{
    std::unordered_map<std::string, std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<State>> allStates;
    std::unordered_map<std::string, std::shared_ptr<PropertyBinding>> bindings;

    for (const auto& item : mStates) {
        states[item.Name] = std::make_shared<State>(item.Name);
        allStates.push_back(states[item.Name]);
    }

    auto tweener = std::make_shared<Tweener>(relPathTweener, tweenerInnerName, states[mStates[0].Name], std::move(allStates));

    for (const auto& item : mTransitions) {
        assert(states.count(item.From));
        auto transition = StateTransition(states.at(item.From), states.at(item.To), std::stof(item.Duration));
        states.at(item.From)->AddStateTransition(transition);
    }

    for (const auto& item : mBindings) {
        bindings[item.BindingName] = CreatePropertyBinding(item);
        tweener->AddPropertyBinding(item.BindingName, bindings.at(item.BindingName));
    }

    for (const auto& item : mProperties) {
        std::shared_ptr<BaseStateProperty> property = CreateProperty(item, bindings);
        states[item.State]->AddStateProperty(property);
    }

    return tweener;
}

#undef TWEENER_START_NODE_NAME
#undef TWEENER_END_NODE_NAME
#undef STATES_START_NODE_NAME
#undef STATES_END_NODE_NAME
#undef STATE_START_NODE_NAME
#undef STATE_END_NODE_NAME
#undef TRANSITIONS_START_NODE_NAME
#undef TRANSITIONS_END_NODE_NAME
#undef TRANSITION_START_NODE_NAME
#undef TRANSITION_END_NODE_NAME
#undef BINDINGS_START_NODE_NAME
#undef BINDINGS_END_NODE_NAME
#undef BINDING_START_NODE_NAME
#undef BINDING_END_NODE_NAME
#undef PROPERTIES_START_NODE_NAME
#undef PROPERTIES_END_NODE_NAME
#undef PROPERTY_START_NODE_NAME
#undef PROPERTY_END_NODE_NAME
} // namespace EngineCore
