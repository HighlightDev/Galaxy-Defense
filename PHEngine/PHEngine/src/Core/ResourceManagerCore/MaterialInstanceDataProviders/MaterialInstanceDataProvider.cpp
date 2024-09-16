#include "MaterialInstanceDataProvider.h"

namespace Resources
{
    int32_t MaterialInstanceDataProvider::GetInstanceId() const
    {
        return mInstanceId;
    }

    bool MaterialInstanceDataProvider::IsInstanceActive() const
    {
        return mIsInstanceActive;
    }
}
