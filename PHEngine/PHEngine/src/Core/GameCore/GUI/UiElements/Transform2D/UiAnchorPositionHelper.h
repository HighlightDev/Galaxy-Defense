#pragma once

#include "UiAnchorType.h"

namespace EngineCore {
namespace GUI {
struct UiAnchorPositionHelper {
    static bool CheckIsAnchorBindingValid(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor);
};
} // namespace GUI
} // namespace EngineCore
