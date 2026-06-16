#include "GameConstants.h"

namespace Game::Constants {
const float c_spaceshipSize = 2.5f;
const int32_t c_spaceshipFontSize = 25;
const int32_t c_extraPathPerSideCount = 2;
const uint32_t c_spawnSpaceshipTimeoutMs = 2000;
const float c_portalSize = 10.0f;
const uint32_t c_dmgTextShowDuration = 2500;
const uint32_t c_shakeDurationMs = 1000;
const glm::vec3 c_barrierPillarScale = glm::vec3(6.0f, 12.0f, 6.0f);
const float c_barrierExclusionRadiusFromDestination = 10.0f;
const glm::vec3 c_lootSize = glm::vec3(1.0f);
const float c_lootTweenTimeSec = 1.0f;
const float c_outlineThickness = 3.0f;

} // namespace Game::Constants

namespace Game::Constants::GravityBombMissile {
// Combat (flying) phase
const float c_combatMeshScale = 1.5f;
const float c_combatUvScale = 0.5f;
const glm::vec3 c_combatOutlineColor = glm::vec3(0.647f, 0.215f, 0.99f);
const float c_combatColliderRadius = 3.0f;
const float c_combatSpeed = 80.0f;
// Singularity (explosion) phase
const float c_singularityMeshScale = 1.5f;
const float c_materialMulCoef = 2.5f;
const float c_singularitySpeed = 80.0f;
const float c_hitboxColliderRadius = 5.0f;
const float c_soundGain = 0.2f;
// Accretion particles
const size_t c_accretionParticleCount = 120;
const float c_accretionEmitRadius = 4.0f;
const size_t c_accretionThetaSlices = 12;
const float c_accretionLifeTime = 3.0f;
const float c_accretionSizeBegin = 1.2f;
const float c_accretionSizeEnd = 0.0f;
const float c_accretionInwardSpeed = 1.5f;
const glm::vec4 c_accretionColorBegin = glm::vec4(0.6f, 0.3f, 1.0f, 1.0f);
const glm::vec4 c_accretionColorEnd = glm::vec4(0.2f, 0.4f, 1.0f, 0.0f);
// Collapse burst particles
const size_t c_burstParticleCount = 400;
const float c_burstEmitRadius = 0.5f;
const size_t c_burstThetaSlices = 8;
const float c_burstLifeTime = 0.8f;
const float c_burstSizeBegin = 0.8f;
const float c_burstSizeEnd = 0.0f;
const float c_burstRadialSpeed = 12.0f;
const glm::vec4 c_burstColorBegin = glm::vec4(0.85f, 0.7f, 1.0f, 1.0f);
const glm::vec4 c_burstColorEnd = glm::vec4(0.4f, 0.3f, 0.9f, 0.0f);
// Orbital rings
const size_t c_ringMaxVertices = 128;
const float c_ringOpacity = 1.0f;
const float c_ringLineWidth = 0.25f;
const int32_t c_ringSegments = 48;
const glm::vec3 c_ringA_TumbleAxis = glm::vec3(1.0f, 0.0f, 0.2f);
const float c_ringA_Radius = 2.0f;
const float c_ringA_SpinSpeed = 120.0f;
const glm::vec3 c_ringA_Color = glm::vec3(0.35f, 0.1f, 0.6f);
const glm::vec3 c_ringA_GlowColor = glm::vec3(0.8f, 0.4f, 1.0f);
const glm::vec3 c_ringB_TumbleAxis = glm::vec3(0.2f, 0.0f, 1.0f);
const float c_ringB_Radius = 2.5f;
const float c_ringB_SpinSpeed = -80.0f;
const glm::vec3 c_ringB_Color = glm::vec3(0.1f, 0.25f, 0.6f);
const glm::vec3 c_ringB_GlowColor = glm::vec3(0.4f, 0.7f, 1.0f);
// Tethers
const float c_tetherOpacity = 1.0f;
const glm::vec3 c_tetherColor = glm::vec3(0.3f, 0.1f, 0.55f);
const glm::vec3 c_tetherGlowColor = glm::vec3(0.75f, 0.35f, 1.0f);
// Gravity-well grid
const size_t c_gridMaxVertices = 1024;
const float c_gridOpacity = 0.6f;
const glm::vec3 c_gridColor = glm::vec3(0.1f, 0.15f, 0.4f);
const glm::vec3 c_gridGlowColor = glm::vec3(0.35f, 0.5f, 1.0f);
const float c_gridLineWidth = 0.15f;
const float c_gridHalfExtent = 18.0f;
const int32_t c_gridLineCount = 11;
const int32_t c_gridSegments = 16;
const float c_gridDeformStrength = 18.0f;
const float c_gridDipStrength = 7.0f;
const float c_gridSoftening = 2.0f;
} // namespace Game::Constants::GravityBombMissile

namespace Game::Constants::BombMissile {
const float c_uvScale = 0.5f;
const glm::vec3 c_outlineColor = glm::vec3(1.0f, 0.0f, 0.0f);
const float c_meshScale = 1.5f;
const float c_speed = 100.0f;
const float c_soundGain = 0.2f;
const float c_colliderRadius = 3.0f;
} // namespace Game::Constants::BombMissile

namespace Game::Constants::FreezingMissile {
const float c_uvScale = 0.5f;
const glm::vec3 c_outlineColor = glm::vec3(0.0f, 0.0f, 1.0f);
const float c_meshScale = 1.5f;
const float c_particleOpacity = 1.0f;
const float c_particleClipRadius = 0.35f;
const size_t c_particleCount = 500;
const float c_emitRadius = 3.0f;
const int32_t c_thetaSlices = 10;
const glm::vec3 c_velocityDirection = glm::vec3(0.0f, -0.5f, 0.0f);
const float c_velocitySpeed = 15.0f;
const glm::vec4 c_colorBegin = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
const glm::vec4 c_colorEnd = glm::vec4(0.3f, 0.3f, 0.7f, 1.0f);
const float c_lifeTime = 1.5f;
const float c_sizeBegin = 0.4f;
const float c_sizeEnd = 0.1f;
const float c_speed = 100.0f;
const float c_soundGain = 0.2f;
const float c_colliderRadius = 3.0f;
} // namespace Game::Constants::FreezingMissile

namespace Game::Constants::ElectroRay {
const glm::vec3 c_beamGlowColor = glm::vec3(0.8f, 0.2f, 0.8f);
const glm::vec3 c_beamMainColor = glm::vec3(0.8f, 1.0f, 0.2f);
const float c_beamThickness = 1.0f;
const int c_beamCount = 2;
const float c_beamJitter = 0.7f;
const float c_beamUpdateFrequency = 0.05f;
const float c_dynamicUpdateFrequency = 0.05f;
const float c_dynamicJitterAmount = 0.3f;
const float c_dynamicAnimationSpeed = 2.0f;
const float c_originSpeed = 65.0f;
const float c_destinationSpeed = 75.0f;
} // namespace Game::Constants::ElectroRay

namespace Game::Constants::FreezingRay {
const glm::vec3 c_beamGlowColor = glm::vec3(0.8f, 0.2f, 1.0f);
const glm::vec3 c_beamMainColor = glm::vec3(0.4f, 0.2f, 1.0f);
const float c_beamThickness = 1.0f;
const int c_beamCount = 2;
const float c_beamJitter = 0.2f;
const float c_beamUpdateFrequency = 0.1f;
} // namespace Game::Constants::FreezingRay

namespace Game::Constants::ElectroRayChain {
const glm::vec3 c_beamGlowColor = glm::vec3(0.8f, 0.2f, 0.8f);
const glm::vec3 c_beamMainColor = glm::vec3(0.8f, 1.0f, 0.2f);
const float c_beamThickness = 1.0f;
const int c_beamCount = 2;
const float c_beamJitter = 0.7f;
const float c_beamUpdateFrequency = 0.05f;
const float c_dynamicUpdateFrequency = 0.05f;
const float c_dynamicJitterAmount = 0.3f;
const float c_dynamicAnimationSpeed = 2.0f;
} // namespace Game::Constants::ElectroRayChain

namespace Game::Constants::Asteroid {
const float c_uvScale = 1.0f;
const float c_speed = 5.0f;
const glm::vec3 c_movementDirection = glm::vec3(1.0f, 0.0f, 0.0f);
const float c_colliderRadius = 3.0f;
} // namespace Game::Constants::Asteroid

namespace Game::Constants::BackgroundPlanets {
const int32_t c_sortOrderValue = -10000;
const float c_speed = 8.5f;
const glm::vec3 c_movementDirection = glm::vec3(0.0f, 0.0f, -1.0f);
} // namespace Game::Constants::BackgroundPlanets

namespace Game::Constants::Barrier {
const glm::vec3 c_albedo = glm::vec3(1.0f, 1.0f, 0.0f);
const float c_metallicValue = 1.8f;
const float c_roughnessValue = 0.5f;
const glm::vec3 c_beamGlowColor = glm::vec3(0.2f, 1.0f, 1.0f);
const glm::vec3 c_beamMainColor = glm::vec3(0.6f, 0.4f, 1.0f);
const uint32_t c_pillarHealth = 50;
const uint32_t c_protoFontSize = 16;
const glm::vec3 c_protoTextColor = glm::vec3(0.3f, 0.5f, 1.0f);
const glm::ivec2 c_protoPadding = glm::ivec2(50);
} // namespace Game::Constants::Barrier

namespace Game::Constants::Loot {
const glm::vec3 c_albedo = glm::vec3(0.0f, 1.0f, 1.0f);
const float c_metallicValue = 0.5f;
const float c_roughnessValue = 0.5f;
const float c_opacity = 1.0f;
const glm::vec3 c_movementDirection = glm::vec3(0.0f, -1.0f, 0.0f);
const float c_fallDistance = 5.0f;
const float c_bounceAmplitude = 0.5f;
const float c_bounceDuration = 0.3f;
const float c_dropSpeed = 16.0f;
const float c_colliderRadiusMultiplier = 1.0f;
const size_t c_particleCount = 50;
const float c_emitRadius = 0.5f;
const size_t c_thetaSlices = 5;
const float c_particleMinLifeTime = 0.4f;
const float c_particleMaxLifeTime = 0.9f;
const float c_particleSpeed = 3.5f;
const float c_sizeBegin = 0.4f;
const float c_sizeEnd = 0.1f;
const glm::vec4 c_colorBegin = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 c_colorEnd = glm::vec4(0.0f, 0.5f, 0.8f, 0.0f);
} // namespace Game::Constants::Loot

namespace Game::Constants::SpawnPortal {
const int32_t c_sortOrderValue = -1000;
} // namespace Game::Constants::SpawnPortal

namespace Game::Constants::Fighter {
const int32_t c_healthRandomRange = 10;
const int32_t c_healthBase = 30;
const int32_t c_level = 1;
const float c_shootRadius = 10.0f;
const int32_t c_cooldownMs = 1000;
const float c_uvScale = 1.0f;
const float c_speed = 10.0f;
const float c_colliderRadiusMultiplier = 0.5f;
const float c_particleOpacity = 1.0f;
const float c_particleClipRadius = 0.35f;
const size_t c_particleCount = 500;
const float c_emitRadius = 5.0f;
const int32_t c_thetaSlices = 10;
const float c_particleLifeTime = 2.5f;
const glm::vec4 c_particleColorBegin = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);
const glm::vec4 c_particleColorEnd = glm::vec4(1.0f, 0.2f, 0.02f, 1.0f);
const float c_particleSizeBegin = 0.4f;
const float c_particleSizeEnd = 0.1f;
const glm::vec3 c_particleVelocityDirection = glm::vec3(0.0f, -0.5f, 0.0f);
const float c_particleVelocitySpeed = 15.0f;
const float c_lightRadius = 100.0f;
const glm::vec3 c_lightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 c_lightDiffuse = glm::vec3(0.4f, 0.1f, 0.1f);
const glm::vec3 c_lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
const glm::vec3 c_uiTextColor = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::ivec2 c_uiPadding = glm::ivec2(50);
const glm::vec3 c_engineMeshPosition = glm::vec3(0.0f, -0.05f, 1.2f);
const glm::vec3 c_engineMeshRotation = glm::vec3(90.0f, 0.0f, 0.0f);
const glm::vec3 c_engineMeshScale = glm::vec3(0.2f, 1.1f, 0.2f);
} // namespace Game::Constants::Fighter

namespace Game::Constants::WeakSpaceShip {
const int32_t c_healthRandomRange = 10;
const int32_t c_healthBase = 20;
const float c_uvScale = 1.0f;
const float c_speed = 10.0f;
const float c_colliderRadiusMultiplier = 0.5f;
const float c_particleOpacity = 1.0f;
const float c_particleClipRadius = 0.35f;
const size_t c_particleCount = 500;
const float c_emitRadius = 5.0f;
const int32_t c_thetaSlices = 10;
const float c_particleLifeTime = 2.5f;
const glm::vec4 c_particleColorBegin = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);
const glm::vec4 c_particleColorEnd = glm::vec4(1.0f, 0.2f, 0.02f, 1.0f);
const float c_particleSizeBegin = 0.4f;
const float c_particleSizeEnd = 0.1f;
const glm::vec3 c_particleVelocityDirection = glm::vec3(0.0f, -0.5f, 0.0f);
const float c_particleVelocitySpeed = 15.0f;
const float c_lightRadius = 100.0f;
const glm::vec3 c_lightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 c_lightDiffuse = glm::vec3(0.4f, 0.1f, 0.1f);
const glm::vec3 c_lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
const glm::vec3 c_uiTextColor = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::ivec2 c_uiPadding = glm::ivec2(50);
const glm::vec3 c_engineMeshPosition = glm::vec3(0.0f, -0.05f, 1.2f);
const glm::vec3 c_engineMeshRotation = glm::vec3(90.0f, 0.0f, 0.0f);
const glm::vec3 c_engineMeshScale = glm::vec3(0.2f, 1.1f, 0.2f);
} // namespace Game::Constants::WeakSpaceShip

namespace Game::Constants::SpaceStation {
const float c_uvScale = 1.0f;
const float c_colliderRadiusMultiplier = 0.5f;
const glm::vec3 c_radiusMarkerColor = glm::vec3(0.0f, 0.2f, 1.0f);
const float c_radiusMeshBaseY = -2.0f;
const float c_radiusMeshIndexYStep = 0.01f;
const int32_t c_radiusMeshSortOrderBase = -100;
} // namespace Game::Constants::SpaceStation
