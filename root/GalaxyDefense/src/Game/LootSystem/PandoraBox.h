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

    bool CheckIfLootDrops();

private:

    explicit PandoraBox();
};
} // namespace Game
