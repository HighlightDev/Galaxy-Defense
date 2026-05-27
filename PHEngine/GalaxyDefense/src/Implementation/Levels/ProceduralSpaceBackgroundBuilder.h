#pragma once

#include <memory>

namespace EngineCore {
class Actor;
class Scene;
} // namespace EngineCore

namespace Game {

// Per-level switches for the procedural space background.
struct SpaceBackgroundConfig {
    // Draw the sphere-shaded planet at all. The starfield + nebula are
    // always drawn; turn the planet off in scenes with a movable camera
    // (e.g. CombatLevel) where a stationary planet would feel pasted on.
    bool includePlanet = false;
    // Tilted ring around the planet. Only visible when includePlanet is true.
    bool includeRing = false;
};

// Builds a full-screen procedural space background and attaches it as a
// BillboardComponent to `targetActor`. The billboard uses identity view /
// projection matrices (same trick as c_spaceBackgroundBillboard in
// CombatLevel) so the quad always covers the entire viewport regardless of
// camera. All visual coefficients — random star/sparkle offset, nebula and
// planet palettes, planet position and radius — are randomised on every call
// so successive level loads show a freshly tinted, freshly arranged cosmos.
void AttachProceduralSpaceBackground(
    const std::shared_ptr<EngineCore::Scene>& scene,
    const std::shared_ptr<EngineCore::Actor>& targetActor,
    const SpaceBackgroundConfig& config);

} // namespace Game
