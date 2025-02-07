#include "UiAnchorPositionHelper.h"

#include <unordered_map>
#include <unordered_set>

namespace EngineCore {
namespace GUI {
bool UiAnchorPositionHelper::CheckIsAnchorBindingValid(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor)
{
    static const std::unordered_map<eUiAnchor, std::unordered_set<eUiAnchor>> validBindings
        = {{eUiAnchor::LEFT, {eUiAnchor::LEFT, eUiAnchor::RIGHT}},
           {eUiAnchor::RIGHT, {eUiAnchor::LEFT, eUiAnchor::RIGHT}},
           {eUiAnchor::TOP, {eUiAnchor::TOP, eUiAnchor::BOTTOM}},
           {eUiAnchor::BOTTOM, {eUiAnchor::TOP, eUiAnchor::BOTTOM}},
           {eUiAnchor::VERTICAL_CENTER, {eUiAnchor::VERTICAL_CENTER}},
           {eUiAnchor::HORIZONTAL_CENTER, {eUiAnchor::HORIZONTAL_CENTER}}};

    return validBindings.count(srcAnchor) ? validBindings.at(srcAnchor).count(dstAnchor) : false;
}
} // namespace GUI
} // namespace EngineCore
