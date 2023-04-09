#include "UiAnchorData.h"

namespace EngineCore
{
    namespace GUI
    {
        UiAnchorData::UiAnchorData()
            : mDstAnchor(eUiAnchor::NONE),
              mDstUiItemName(""),
              mSrcAnchorMargin(0)
        {
        }

        UiAnchorData::UiAnchorData(const eUiAnchor dstAnchor, const std::string &dstUiItemName, const int32_t srcAnchorMargin)
            : mDstAnchor(dstAnchor),
              mDstUiItemName(dstUiItemName),
              mSrcAnchorMargin(srcAnchorMargin)
        {
        }

        eUiAnchor UiAnchorData::GetDstAnchor() const
        {
            return mDstAnchor;
        }

        std::string UiAnchorData::GetDstUiItemName() const
        {
            return mDstUiItemName;
        }

        int32_t UiAnchorData::GetSrcAnchorMargin() const
        {
            return mSrcAnchorMargin;
        }

        void UiAnchorData::SetDstAnchor(const eUiAnchor dstAnchor)
        {
            mDstAnchor = dstAnchor;
        }

        void UiAnchorData::SetDstUiItemName(const std::string &name)
        {
            mDstUiItemName = name;
        }

        void UiAnchorData::SetSrcAnchorMargin(const int32_t srcMargin)
        {
            mSrcAnchorMargin = srcMargin;
        }
    }
}