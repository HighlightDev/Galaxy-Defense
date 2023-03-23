#include "UiAnchorLuaData.h"

namespace EngineCore
{
    namespace Scripts
    {
        UiAnchorLuaData::UiAnchorLuaData()
            : mDstAnchor(eUiAnchor::NONE),
              mDstLuaProxyId(-1),
              mSrcAnchorMargin(0)
        {
        }

        eUiAnchor UiAnchorLuaData::GetDstAnchor() const
        {
            return mDstAnchor;
        }

        int32_t UiAnchorLuaData::GetDstUiItemLuaProxyId() const
        {
            return mDstLuaProxyId;
        }

        int32_t UiAnchorLuaData::GetSrcAnchorMargin() const
        {
            return mSrcAnchorMargin;
        }

        void UiAnchorLuaData::SetDstAnchor(const eUiAnchor dstAnchor)
        {
            mDstAnchor = dstAnchor;
        }

        void UiAnchorLuaData::SetDstUiItemLuaProxyId(const int32_t id)
        {
            mDstLuaProxyId = id;
        }

        void UiAnchorLuaData::SetSrcAnchorMargin(const int32_t srcMargin)
        {
            mSrcAnchorMargin = srcMargin;
        }
    }
}