#include "PandoraBox.h"

#include "Core/CommonCore/Random.h"
#include "Game/DataProviders/GameConstants.h"

namespace Game {
PandoraBox::PandoraBox()
    : mPRDData(Game::Constants::c_lootChanceCoef, Game::Constants::c_numberOfPRDIterations)
{
}

void PandoraBox::ResetState()
{
    mPRDData.Reset();
}

bool PandoraBox::CheckIfLootDrops()
{
    bool isDrop = false;
    if (mPRDData.N >= mPRDData.NMax) {
        isDrop = true;
    } else {
        const auto currentChance = mPRDData.C * mPRDData.N;
        isDrop = Random::Float() < currentChance;
    }
    mPRDData.N = isDrop ? 1 : (mPRDData.N + 1);
    return isDrop;
}
} // namespace Game
