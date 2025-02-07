#pragma once

#include <string>

namespace EngineCore {
namespace GUI {
class AnimationData;
class AnimationSequence;
} // namespace GUI
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class IAnimatableLuaProxy {
public:
    virtual void AddAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationData& animationData) = 0;

    virtual void
    AddSequenceAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationSequence& animationSequence)
        = 0;

    virtual void StartAnimation(const std::string& animationName) = 0;

    virtual void StartSequenceAnimation(const std::string& animationSequenceName) = 0;
};
} // namespace Scripts
} // namespace EngineCore
