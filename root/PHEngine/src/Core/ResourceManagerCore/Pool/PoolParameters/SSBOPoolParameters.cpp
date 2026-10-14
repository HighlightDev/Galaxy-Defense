#include "SSBOPoolParameters.h"

namespace Resources {

uint32_t SSBOPoolParameters::unique_index = 0;

SSBOPoolParameters::SSBOPoolParameters(uint32_t bytesToAllocate, uint32_t bindingPoint, uint32_t flags, bool bUniqueResource)
    : bytesToAllocate(bytesToAllocate)
    , bindingPoint(bindingPoint)
    , flags(flags)
    , bUniqueResource(bUniqueResource)
    , uniqueIndex(unique_index++)
{
}

bool SSBOPoolParameters::operator==(const SSBOPoolParameters& other) const
{
    return this->bytesToAllocate == other.bytesToAllocate && this->bindingPoint == other.bindingPoint
        && this->flags == other.flags
        && ((this->bUniqueResource || other.bUniqueResource) ? this->uniqueIndex == other.uniqueIndex : true);
}
} // namespace Resources
