#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdint.h>

#include <cstddef>
#include <cstdint>

namespace Game::Constants {
extern const float c_spaceshipSize;
extern const int32_t c_spaceshipFontSize;
extern const int32_t c_extraPathPerSideCount;
extern const uint32_t c_spawnSpaceshipTimeoutMs;
extern const float c_portalSize;
extern const uint32_t c_dmgTextShowDuration;
extern const uint32_t c_shakeDurationMs;
extern const glm::vec3 c_barrierPillarScale;
extern const float c_barrierExclusionRadiusFromDestination;
extern const glm::vec3 c_lootSize;
extern const float c_lootTweenTimeSec;
extern const float c_outlineThickness;
extern const float c_lootDropChance;
} // namespace Game::Constants

namespace Game::Constants::GravityBombMissile {
// Combat (flying) phase
extern const float c_combatMeshScale;
extern const float c_combatUvScale;
extern const glm::vec3 c_combatOutlineColor;
extern const float c_combatColliderRadius;
extern const float c_combatSpeed;
// Singularity (explosion) phase
extern const float c_singularityMeshScale;
extern const float c_materialMulCoef;
extern const float c_singularitySpeed;
extern const float c_hitboxColliderRadius;
extern const float c_soundGain;
// Accretion particles
extern const size_t c_accretionParticleCount;
extern const float c_accretionEmitRadius;
extern const size_t c_accretionThetaSlices;
extern const float c_accretionLifeTime;
extern const float c_accretionSizeBegin;
extern const float c_accretionSizeEnd;
extern const float c_accretionInwardSpeed;
extern const glm::vec4 c_accretionColorBegin;
extern const glm::vec4 c_accretionColorEnd;
// Collapse burst particles
extern const size_t c_burstParticleCount;
extern const float c_burstEmitRadius;
extern const size_t c_burstThetaSlices;
extern const float c_burstLifeTime;
extern const float c_burstSizeBegin;
extern const float c_burstSizeEnd;
extern const float c_burstRadialSpeed;
extern const glm::vec4 c_burstColorBegin;
extern const glm::vec4 c_burstColorEnd;
// Orbital rings
extern const size_t c_ringMaxVertices;
extern const float c_ringOpacity;
extern const float c_ringLineWidth;
extern const int32_t c_ringSegments;
extern const glm::vec3 c_ringA_TumbleAxis;
extern const float c_ringA_Radius;
extern const float c_ringA_SpinSpeed;
extern const glm::vec3 c_ringA_Color;
extern const glm::vec3 c_ringA_GlowColor;
extern const glm::vec3 c_ringB_TumbleAxis;
extern const float c_ringB_Radius;
extern const float c_ringB_SpinSpeed;
extern const glm::vec3 c_ringB_Color;
extern const glm::vec3 c_ringB_GlowColor;
// Tethers
extern const float c_tetherOpacity;
extern const glm::vec3 c_tetherColor;
extern const glm::vec3 c_tetherGlowColor;
// Gravity-well grid
extern const size_t c_gridMaxVertices;
extern const float c_gridOpacity;
extern const glm::vec3 c_gridColor;
extern const glm::vec3 c_gridGlowColor;
extern const float c_gridLineWidth;
extern const float c_gridHalfExtent;
extern const int32_t c_gridLineCount;
extern const int32_t c_gridSegments;
extern const float c_gridDeformStrength;
extern const float c_gridDipStrength;
extern const float c_gridSoftening;
} // namespace Game::Constants::GravityBombMissile

namespace Game::Constants::PlasmaBombMissile {
// Combat (flying) phase
extern const float c_combatMeshScale;
extern const float c_combatUvScale;
extern const glm::vec3 c_combatOutlineColor;
extern const float c_combatColliderRadius;
extern const float c_combatSpeed;
extern const float c_soundGain;
// Small semi-transparent plasma sphere that pops at the impact point
extern const float c_coreMeshScale;
extern const float c_materialMulCoef;
extern const glm::vec3 c_coreTintColor;
extern const glm::vec3 c_coreGlowColor;
extern const float c_sphereOpacity;
// Damage-over-time burn applied to the ship on hit
extern const float c_dotDamagePerSecond;
extern const float c_dotDurationSec;
// Scorch burst: short-lived sparks fired radially from the impact point when the plasma sears the target
extern const size_t c_burnParticleCount;
extern const float c_burnEmitRadius;
extern const size_t c_burnThetaSlices;
extern const float c_burnLifeTime;
extern const float c_burnSizeBegin;
extern const float c_burnSizeEnd;
extern const float c_burnRadialSpeed;
extern const glm::vec4 c_burnColorBegin;
extern const glm::vec4 c_burnColorEnd;
// Plasma trail (tapered, fading tail following the flying missile)
extern const size_t c_trailMaxVertices;
extern const size_t c_trailMaxPoints;
extern const float c_trailMinPointSpacing;
extern const float c_trailLineWidth;
extern const float c_trailOpacity;
extern const glm::vec3 c_trailColor;
extern const glm::vec3 c_trailGlowColor;
// Seconds over which the frozen trail tapers to nothing after impact. Kept just under the 0.6s Impact->Dissipate
// window so the trail is already invisible by the time the missile disables (no abrupt pop).
extern const float c_trailFadeOutSec;
} // namespace Game::Constants::PlasmaBombMissile

namespace Game::Constants::BombMissile {
extern const float c_uvScale;
extern const glm::vec3 c_outlineColor;
extern const float c_meshScale;
extern const float c_speed;
extern const float c_soundGain;
extern const float c_colliderRadius;
} // namespace Game::Constants::BombMissile

namespace Game::Constants::FreezingMissile {
extern const float c_uvScale;
extern const glm::vec3 c_outlineColor;
extern const float c_meshScale;
extern const float c_particleOpacity;
extern const float c_particleClipRadius;
extern const size_t c_particleCount;
extern const float c_emitRadius;
extern const int32_t c_thetaSlices;
extern const glm::vec3 c_velocityDirection;
extern const float c_velocitySpeed;
extern const glm::vec4 c_colorBegin;
extern const glm::vec4 c_colorEnd;
extern const float c_lifeTime;
extern const float c_sizeBegin;
extern const float c_sizeEnd;
extern const float c_speed;
extern const float c_soundGain;
extern const float c_colliderRadius;
} // namespace Game::Constants::FreezingMissile

namespace Game::Constants::ElectroRay {
extern const glm::vec3 c_beamGlowColor;
extern const glm::vec3 c_beamMainColor;
extern const float c_beamThickness;
extern const int c_beamCount;
extern const float c_beamJitter;
extern const float c_beamUpdateFrequency;
extern const float c_dynamicUpdateFrequency;
extern const float c_dynamicJitterAmount;
extern const float c_dynamicAnimationSpeed;
extern const float c_originSpeed;
extern const float c_destinationSpeed;
} // namespace Game::Constants::ElectroRay

namespace Game::Constants::FreezingRay {
extern const glm::vec3 c_beamGlowColor;
extern const glm::vec3 c_beamMainColor;
extern const float c_beamThickness;
extern const int c_beamCount;
extern const float c_beamJitter;
extern const float c_beamUpdateFrequency;
} // namespace Game::Constants::FreezingRay

namespace Game::Constants::ElectroRayChain {
extern const glm::vec3 c_beamGlowColor;
extern const glm::vec3 c_beamMainColor;
extern const float c_beamThickness;
extern const int c_beamCount;
extern const float c_beamJitter;
extern const float c_beamUpdateFrequency;
extern const float c_dynamicUpdateFrequency;
extern const float c_dynamicJitterAmount;
extern const float c_dynamicAnimationSpeed;
} // namespace Game::Constants::ElectroRayChain

namespace Game::Constants::Asteroid {
extern const float c_uvScale;
extern const float c_speed;
extern const glm::vec3 c_movementDirection;
extern const float c_colliderRadius;
} // namespace Game::Constants::Asteroid

namespace Game::Constants::BackgroundPlanets {
extern const int32_t c_sortOrderValue;
extern const float c_speed;
extern const glm::vec3 c_movementDirection;
} // namespace Game::Constants::BackgroundPlanets

namespace Game::Constants::Barrier {
extern const glm::vec3 c_albedo;
extern const float c_metallicValue;
extern const float c_roughnessValue;
extern const glm::vec3 c_beamGlowColor;
extern const glm::vec3 c_beamMainColor;
extern const uint32_t c_pillarHealth;
extern const uint32_t c_protoFontSize;
extern const glm::vec3 c_protoHealTextColor;
extern const glm::vec3 c_protoDamageTextColor;
extern const glm::ivec2 c_protoPadding;
} // namespace Game::Constants::Barrier

namespace Game::Constants::Loot {
extern const glm::vec3 c_albedo;
extern const float c_metallicValue;
extern const float c_roughnessValue;
extern const float c_opacity;
extern const glm::vec3 c_movementDirection;
extern const float c_fallDistance;
extern const float c_bounceAmplitude;
extern const float c_bounceDuration;
extern const float c_dropSpeed;
extern const float c_colliderRadiusMultiplier;
extern const size_t c_particleCount;
extern const float c_emitRadius;
extern const size_t c_thetaSlices;
extern const float c_particleMinLifeTime;
extern const float c_particleMaxLifeTime;
extern const float c_particleSpeed;
extern const float c_sizeBegin;
extern const float c_sizeEnd;
extern const glm::vec4 c_colorBegin;
extern const glm::vec4 c_colorEnd;
} // namespace Game::Constants::Loot

namespace Game::Constants::SpawnPortal {
extern const int32_t c_sortOrderValue;
} // namespace Game::Constants::SpawnPortal

namespace Game::Constants::Fighter {
extern const int32_t c_healthRandomRange;
extern const int32_t c_healthBase;
extern const int32_t c_level;
extern const float c_shootRadius;
extern const int32_t c_cooldownMs;
extern const float c_uvScale;
extern const float c_speed;
extern const float c_colliderRadiusMultiplier;
extern const float c_particleOpacity;
extern const float c_particleClipRadius;
extern const size_t c_particleCount;
extern const float c_emitRadius;
extern const int32_t c_thetaSlices;
extern const float c_particleLifeTime;
extern const glm::vec4 c_particleColorBegin;
extern const glm::vec4 c_particleColorEnd;
extern const float c_particleSizeBegin;
extern const float c_particleSizeEnd;
extern const glm::vec3 c_particleVelocityDirection;
extern const float c_particleVelocitySpeed;
extern const float c_lightRadius;
extern const glm::vec3 c_lightAmbient;
extern const glm::vec3 c_lightDiffuse;
extern const glm::vec3 c_lightSpecular;
extern const glm::vec3 c_uiTextColor;
extern const glm::ivec2 c_uiPadding;
extern const glm::vec3 c_engineMeshPosition;
extern const glm::vec3 c_engineMeshRotation;
extern const glm::vec3 c_engineMeshScale;
} // namespace Game::Constants::Fighter

namespace Game::Constants::WeakSpaceShip {
extern const int32_t c_healthRandomRange;
extern const int32_t c_healthBase;
extern const float c_uvScale;
extern const float c_speed;
extern const float c_colliderRadiusMultiplier;
extern const float c_particleOpacity;
extern const float c_particleClipRadius;
extern const size_t c_particleCount;
extern const float c_emitRadius;
extern const int32_t c_thetaSlices;
extern const float c_particleLifeTime;
extern const glm::vec4 c_particleColorBegin;
extern const glm::vec4 c_particleColorEnd;
extern const float c_particleSizeBegin;
extern const float c_particleSizeEnd;
extern const glm::vec3 c_particleVelocityDirection;
extern const float c_particleVelocitySpeed;
extern const float c_lightRadius;
extern const glm::vec3 c_lightAmbient;
extern const glm::vec3 c_lightDiffuse;
extern const glm::vec3 c_lightSpecular;
extern const glm::vec3 c_uiTextColor;
extern const glm::ivec2 c_uiPadding;
extern const glm::vec3 c_engineMeshPosition;
extern const glm::vec3 c_engineMeshRotation;
extern const glm::vec3 c_engineMeshScale;
} // namespace Game::Constants::WeakSpaceShip

namespace Game::Constants::SpaceStation {
extern const float c_uvScale;
extern const float c_colliderRadiusMultiplier;
extern const glm::vec3 c_radiusMarkerColor;
extern const float c_radiusMeshBaseY;
extern const float c_radiusMeshIndexYStep;
extern const int32_t c_radiusMeshSortOrderBase;
} // namespace Game::Constants::SpaceStation
