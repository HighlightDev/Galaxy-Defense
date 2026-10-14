#pragma once

#include <cstdint>

namespace EngineCore {
namespace GUI {
// Rude rendering order of UI: subtrees of different layers are not mixed (layer is the first element of z-path).
// Inside the layer, the order is determined by the widget tree and local z. INHERIT — take the layer from the parent.
enum class eUiLayer : int32_t { INHERIT = -1, BACKGROUND = 0, HUD = 100, PANEL = 200, OVERLAY = 300, MODAL = 400, TOOLTIP = 500 };
} // namespace GUI
} // namespace EngineCore
