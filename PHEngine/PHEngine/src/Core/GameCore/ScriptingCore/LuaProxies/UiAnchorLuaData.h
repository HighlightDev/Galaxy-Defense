#pragma once

#include <string>
#include <stdint.h>
#include <cstdint>

#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorType.h"

using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        class UiAnchorLuaData
        {
            eUiAnchor mDstAnchor;
            int32_t mDstLuaProxyId;
            int32_t mSrcAnchorMargin;

        public:
            UiAnchorLuaData();

            eUiAnchor GetDstAnchor() const;

            std::int32_t GetDstUiItemLuaProxyId() const;

            int32_t GetSrcAnchorMargin() const;

            void SetDstAnchor(const eUiAnchor dstAnchor);

            void SetDstUiItemLuaProxyId(const int32_t id);

            void SetSrcAnchorMargin(const int32_t srcMargin);
        };
    }
}