#include "MaterialInstanceDataProvider.h"

namespace Resources {
int32_t MaterialInstanceDataProvider::GetRenderInstanceId() const
{
    return mRenderInstanceId;
}

bool MaterialInstanceDataProvider::IsInstanceActive() const
{
    return mIsInstanceActive;
}
} // namespace Resources
