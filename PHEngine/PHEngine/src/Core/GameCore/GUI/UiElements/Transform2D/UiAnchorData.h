#pragma once

#include <string>
#include <stdint.h>
#include <cstdint>

#include "UiAnchorType.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiAnchorData
        {
            eUiAnchor mDstAnchor;
            std::string mDstUiItemName;
            int32_t mSrcAnchorMargin;

        public:
            explicit UiAnchorData();

            UiAnchorData(const eUiAnchor dstAnchor, const std::string& dstUiItemName, const int32_t srcAnchorMargin);

            eUiAnchor GetDstAnchor() const;

            std::string GetDstUiItemName() const;

            int32_t GetSrcAnchorMargin() const;

            void SetDstAnchor(const eUiAnchor dstAnchor);

            void SetDstUiItemName(const std::string &name);

            void SetSrcAnchorMargin(const int32_t srcMargin);
        };
    }
}