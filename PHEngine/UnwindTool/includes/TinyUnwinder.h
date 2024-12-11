#pragma once

#include <string>

namespace Tools::Unwind
{
    class TinyUnwinder
    {
    public:
        std::string GetStackBacktraceStr() const;
    };
}