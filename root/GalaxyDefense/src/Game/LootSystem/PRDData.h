#pragma once

#include <cstdint>

namespace Game {
/*Pseudo random drop data*/
struct PRDData {
    /*
    Chance coeff - chance increment per iteration, drop chance on N-th iteration is C * N
    Nominal drop chance
    (P)|       C:
    5% |    0.380%
    10%|    1.475%
    15%|    3.222%
    20%|    5.570%
    25%|    8.475%
    */
    const float C;

    /*
    Max number of iterations without drop, if NMax <= N - loot must drop
    */
    const int32_t NMax;

    /*
    Count of iterations without drop
    */
    int32_t N;

    PRDData(const float _C, const int32_t _NMax)
        : C(_C)
        , NMax(_NMax)
        , N(1)
    {
    }

    void Reset()
    {
        N = 1;
    }
};
} // namespace Game