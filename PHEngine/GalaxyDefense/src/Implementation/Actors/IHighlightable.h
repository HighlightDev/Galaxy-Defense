#pragma once

namespace Game {
class IHighlightable {
public:
    virtual void ChangeHighlightState(const bool isHighlightEnabled) = 0;
};
} // namespace Game
