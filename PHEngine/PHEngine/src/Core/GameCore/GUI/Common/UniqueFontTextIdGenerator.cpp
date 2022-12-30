#include "UniqueFontTextIdGenerator.h"
namespace EngineCore
{
    std::atomic<int32_t> UniqueFontTextIdGenerator::s_TotalTextFieldId = 0;

    int32_t UniqueFontTextIdGenerator::GenerateUniqueFontTextId()
    {
        const auto result = s_TotalTextFieldId.load();
        s_TotalTextFieldId.fetch_add(1, std::memory_order::memory_order_seq_cst);
        return result;
    }
}