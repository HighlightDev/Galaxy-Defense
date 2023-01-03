#include "SpaceshipActor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/Components/UiComponents/UiComponent.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

namespace Game
{
    SpaceshipActor::SpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent)
        : Actor(gameObjectName, rootComponent),
          mModifiers(),
          mLifePoints(10),
          mDamageTextFieldWp(),
          mDamageEffectTimePassed(0.0f),
          mDamageEffectDuration(0.5f),
          mDamageTimeProperty(std::make_shared<EngineGOProperty<float>>(0.0f, "p_damageEffect")),
          mFreezingEffectProperty(std::make_shared<EngineGOProperty<float>>(0.0f, "p_freezingEffect")),
          mDamageTextShowDuration(1.5f),
          mDamageTextTimePassed(0.0f),
          mIsDamageEffectActive(false),
          mIsDamageTextActive(false)
    {
        AddEngineProperty(mDamageTimeProperty);
        AddEngineProperty(mFreezingEffectProperty);
    }

    void SpaceshipActor::PostLevelInit()
    {
        Actor::PostLevelInit();

        const auto c_uiComponent = GetComponentsByType<UiComponent>().back();
        const size_t dmgTextFieldId = c_uiComponent->CreateEmptyTextField("nimbus_mono", 10, glm::vec3(1.0f, 0.0f, 0.0f), true, 0.3f, 1, eTextHorizontalAlignmentType::LEFT);
        mDamageTextFieldWp = c_uiComponent->GetTextFieldById(dmgTextFieldId);
    }

    void SpaceshipActor::TriggerSpawn(const glm::vec3 &position)
    {
        SetIsEnabled(true);
        mActivityState = eSpaceshipActivityState::ACTIVE;
        GetMovementComponent()->Teleport(position);
        RestoreLife();
    }

    void SpaceshipActor::TriggerExplosion()
    {
        TriggerDisabled();
    }

    void SpaceshipActor::TriggerDisabled()
    {
        std::for_each(mModifiers.begin(), mModifiers.end(), [](const auto &modifier)
                      { modifier->OnPreRemoved(); });

        mModifiers.clear();
        mActivityState = eSpaceshipActivityState::IDLE;
        SetIsEnabled(false);
    }

    void SpaceshipActor::RemoveExpiredModifiers()
    {
        const auto expiredIt = std::remove_if(mModifiers.begin(), mModifiers.end(), [](const auto &modifier)
                                              { 
                                                const bool isExpired = modifier->IsExpired();
                                                if (isExpired)
                                                {
                                                    modifier->OnPreRemoved();
                                                }
                                                return isExpired; });

        if (mModifiers.end() != expiredIt)
        {
            mModifiers.erase(expiredIt, mModifiers.end());
        }
    }

    void SpaceshipActor::Tick(const float deltaTime)
    {
        Actor::Tick(deltaTime);

        RemoveExpiredModifiers();

        for (const auto &modifier : mModifiers)
        {
            modifier->Tick(deltaTime);
        }

        if (mIsDamageEffectActive)
        {
            const float normDmgEffectTime = glm::clamp(mDamageEffectTimePassed / mDamageEffectDuration, 0.0f, 1.0f);
            mDamageTimeProperty->SetValue(normDmgEffectTime);

            if (mDamageEffectTimePassed < mDamageEffectDuration)
            {
                mDamageEffectTimePassed += deltaTime;
            }
            else
            {
                mIsDamageEffectActive = false;
                mDamageEffectTimePassed = 0.0f;
                mDamageTimeProperty->SetValue(0.0f);
            }
        }

        if (mIsDamageTextActive)
        {
            if (const auto &sceneSp = mSceneOwner.lock())
            {
                if (const auto &dmgTextFieldSp = mDamageTextFieldWp.lock())
                {
                    const auto &spaceShipTranslation = GetRootComponent()->GetTranslation();
                    const auto &mainCameraSp = sceneSp->GetMainCamera();
                    const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                    const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                               clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                               clippedSpaceTranslation.z / clippedSpaceTranslation.w);

                    const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                    dmgTextFieldSp->SetPosition(textureSpaceTranslation - (dmgTextFieldSp->GetScreenSpaceSize().x * 0.5f) + glm::vec2(0.0f, -0.2f));
                }
            }

            if (mDamageTextTimePassed < mDamageTextShowDuration)
            {
                mDamageTextTimePassed += deltaTime;
            }
            else if (const auto &dmgTextFieldSp = mDamageTextFieldWp.lock())
            {
                dmgTextFieldSp->SetVisibility(false);
                mIsDamageTextActive = false;
                mDamageTextTimePassed = 0.0f;
            }
        }
    }

    void SpaceshipActor::TriggerDamageReceived(const size_t dmg)
    {
        if (CheckIsAliveAfterDamage(dmg))
        {
            mIsDamageEffectActive = true;
            mIsDamageTextActive = true;
            mDamageEffectTimePassed = 0.0f;
            mDamageTextTimePassed = 0.0f;

            const auto c_particle = GetComponentsByType<ParticleSystemComponent>().back();
            c_particle->EmitParticles();

            if (const auto &dmgTextFieldSp = mDamageTextFieldWp.lock())
            {
                dmgTextFieldSp->SetText(std::to_string(dmg));
                dmgTextFieldSp->SetVisibility(true);
                dmgTextFieldSp->SetPosition(CalculatePositionForDamageText());
            }
        }
    }

    glm::vec2 SpaceshipActor::CalculatePositionForDamageText() const
    {
        const auto &spaceShipTranslation = GetRootComponent()->GetTranslation();

        if (const auto &sceneSp = mSceneOwner.lock())
        {
            if (const auto &dmgTextFieldSp = mDamageTextFieldWp.lock())
            {
                const auto &mainCameraSp = sceneSp->GetMainCamera();
                const glm::vec4 clippedSpaceTranslation = mainCameraSp->GetConvertedToClippedSpacePosition(glm::vec4(spaceShipTranslation, 1.0f));
                const glm::vec3 ndcTranslation = glm::vec3(clippedSpaceTranslation.x / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.y / clippedSpaceTranslation.w,
                                                           clippedSpaceTranslation.z / clippedSpaceTranslation.w);

                const glm::vec2 textureSpaceTranslation = glm::vec2(ndcTranslation.x * 0.5f + 0.5f, 1.0f - (ndcTranslation.y * 0.5f + 0.5f));
                return (textureSpaceTranslation + glm::vec2(dmgTextFieldSp->GetScreenSpaceSize().x * -0.5f, -0.2f));
            }
        }

        return glm::vec2(spaceShipTranslation.x, spaceShipTranslation.y);
    }

    glm::vec3 SpaceshipActor::GetWorldPosition() const
    {
        return GetRootComponent()->GetTranslation();
    }

    bool SpaceshipActor::IsInsideLevel(const BoundingBox3D &boundingBox) const
    {
        return EngineMath::TestPointInAABB(boundingBox.GetMin(), boundingBox.GetMax(), GetRootComponent()->GetTranslation());
    }

    void SpaceshipActor::AddModifier(const std::shared_ptr<IModifiable> &modifier)
    {
        mModifiers.emplace_back(modifier);
    }

    bool SpaceshipActor::HasModifier(const eModifierType modifierType, const uint64_t creatorObjectId) const
    {
        auto foundIt = std::find_if(mModifiers.begin(), mModifiers.end(), [=](const auto &modifier)
                                    { return (modifierType == modifier->GetModifierType() && creatorObjectId == modifier->CreatorObjectId()); });
        return mModifiers.end() != foundIt;
    }

    bool SpaceshipActor::HasModifier(const eModifierType modifierType) const
    {
        auto foundIt = std::find_if(mModifiers.begin(), mModifiers.end(), [=](const auto &modifier)
                                    { return modifierType == modifier->GetModifierType(); });
        return mModifiers.end() != foundIt;
    }

    std::shared_ptr<IModifiable> SpaceshipActor::GetModifier(const eModifierType modifierType) const
    {
        auto foundIt = std::find_if(mModifiers.begin(), mModifiers.end(), [=](const auto &modifier)
                                    { return modifierType == modifier->GetModifierType(); });
        return mModifiers.end() != foundIt ? *foundIt : nullptr;
    }

    void SpaceshipActor::RemoveModifier(const eModifierType modifierType, const uint64_t creatorObjectId)
    {
        auto removeIt = std::remove_if(mModifiers.begin(), mModifiers.end(), [=](const auto &modifier)
                                       { return (modifierType == modifier->GetModifierType() && creatorObjectId == modifier->CreatorObjectId()); });
        if (mModifiers.end() != removeIt)
        {
            mModifiers.erase(removeIt, mModifiers.end());
        }
    }

    bool SpaceshipActor::CheckIsAliveAfterDamage(const size_t dmg)
    {
        mLifePoints = mLifePoints >= dmg ? mLifePoints - dmg : 0;
        return 0 != mLifePoints;
    }

    void SpaceshipActor::SetDamageDeltaTime(const float deltaTime)
    {
        mDamageEffectTimePassed = deltaTime;
    }

    float SpaceshipActor::GetDamageDeltaTime() const
    {
        return mDamageEffectTimePassed;
    }

    void SpaceshipActor::SetIsDamageReceived(const bool isDamageReceived)
    {
        mIsDamageEffectActive = isDamageReceived;
        mDamageEffectTimePassed = 0.0f;
    }

    bool SpaceshipActor::GetIsDamageReceived() const
    {
        return mIsDamageEffectActive;
    }

    void SpaceshipActor::RestoreLife()
    {
        mLifePoints = 10;
    }

    eSpaceshipActivityState SpaceshipActor::GetSpaceshipActivityState() const
    {
        return mActivityState;
    }

    void SpaceshipActor::SetFreezingEffectValue(const float value)
    {
        mFreezingEffectProperty->SetValue(value);
    }
}