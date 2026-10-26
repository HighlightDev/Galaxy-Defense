#pragma once

#include "Game/DropLootType.h"
#include "PRDData.h"

namespace Game {
class PandoraBox {

    PRDData mPRDData;

public:
    static PandoraBox* GetInstance()
    {
        static PandoraBox pandoraBox;
        return &pandoraBox;
    }

    void ResetState();

    eDropLootType AttemptToPickLootFromTheBox();

private:
    explicit PandoraBox();

    bool CheckIfLootDrops();
};
} // namespace Game
