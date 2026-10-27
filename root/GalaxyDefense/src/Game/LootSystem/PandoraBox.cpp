#include "PandoraBox.h"

#include "Core/CommonCore/Random.h"
#include "Game/DataProviders/GameConstants.h"

#include <array>
#include <numeric>

namespace Game {

namespace {
constexpr std::array<std::pair<eDropLootType, uint32_t>, 1> c_lootWeights = {std::make_pair(eDropLootType::CRYSTALS, 100)};
constexpr uint32_t c_totalWeight = 100;
} // namespace

PandoraBox::PandoraBox()
    : mPRDData(Game::Constants::c_lootChanceCoef, Game::Constants::c_numberOfPRDIterations)
{
}

void PandoraBox::ResetState()
{
    mPRDData.Reset();
}

bool PandoraBox::CheckIfLootDrops(const uint32_t spaceshipDanger)
{
    bool isDrop = false;
    if (mPRDData.N >= mPRDData.NMax) {
        isDrop = true;
    } else {
        constexpr float k = 0.5f;
        // More danger -> more chance for drop
        const auto Ceff = mPRDData.C * std::pow((float)spaceshipDanger / (float)Game::Constants::c_weakSpaceshipDanger, k);
        const auto currentChance = Ceff * mPRDData.N;
        isDrop = Random::Float() < currentChance;
    }
    mPRDData.N = isDrop ? 1 : (mPRDData.N + 1);
    return isDrop;
}

eDropLootType PandoraBox::AttemptToPickLootFromTheBox(const uint32_t spaceshipDanger)
{
    eDropLootType result{eDropLootType::NONE};
    if (CheckIfLootDrops(spaceshipDanger)) {

        const auto target = Random::Float() * c_totalWeight;

        float cumulativeWeight = 0;

        for (const auto& [loot, weight] : c_lootWeights) {
            cumulativeWeight += weight;
            if (cumulativeWeight >= target) {
                result = loot;
                break;
            }
        }

        result = c_lootWeights.back().first;
    }
    return result;
}
} // namespace Game
