#include "ProceduralSpaceBackgroundBuilder.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <cstdint>

using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace Graphics;

namespace Game {

namespace {

// Hand-picked palettes for the nebula triad + matching planet body / highlight.
// Each load picks one uniformly at random so the menu and combat feel different
// between sessions without ever drifting into the muddy / clashy combinations
// that pure HSV randomisation tends to produce.
struct SpacePalette {
    glm::vec3 nebulaA;
    glm::vec3 nebulaB;
    glm::vec3 nebulaC;
    glm::vec3 planetHue;
    glm::vec3 planetHighlight;
};

const std::array<SpacePalette, 5> kPalettes = {
    SpacePalette{// "cyan deep" — original menu look
                 glm::vec3(0.18f, 0.30f, 0.78f),
                 glm::vec3(0.40f, 0.28f, 0.85f),
                 glm::vec3(0.18f, 0.55f, 0.85f),
                 glm::vec3(0.12f, 0.30f, 0.48f),
                 glm::vec3(0.49f, 0.77f, 0.94f)},
    SpacePalette{// "amethyst"
                 glm::vec3(0.45f, 0.20f, 0.78f),
                 glm::vec3(0.78f, 0.30f, 0.85f),
                 glm::vec3(0.35f, 0.18f, 0.62f),
                 glm::vec3(0.35f, 0.15f, 0.50f),
                 glm::vec3(0.85f, 0.55f, 0.95f)},
    SpacePalette{// "ember"
                 glm::vec3(0.78f, 0.35f, 0.20f),
                 glm::vec3(0.95f, 0.55f, 0.25f),
                 glm::vec3(0.55f, 0.20f, 0.35f),
                 glm::vec3(0.45f, 0.20f, 0.10f),
                 glm::vec3(0.95f, 0.70f, 0.40f)},
    SpacePalette{// "verdant"
                 glm::vec3(0.20f, 0.55f, 0.35f),
                 glm::vec3(0.30f, 0.78f, 0.55f),
                 glm::vec3(0.18f, 0.45f, 0.60f),
                 glm::vec3(0.10f, 0.35f, 0.20f),
                 glm::vec3(0.45f, 0.85f, 0.60f)},
    SpacePalette{// "rose"
                 glm::vec3(0.75f, 0.30f, 0.55f),
                 glm::vec3(0.55f, 0.20f, 0.75f),
                 glm::vec3(0.85f, 0.50f, 0.70f),
                 glm::vec3(0.45f, 0.15f, 0.35f),
                 glm::vec3(0.95f, 0.65f, 0.85f)},
};

// Map a uniform [0,1) sample to an integer index in [0, count).
int32_t RandomIndex(const int32_t count)
{
    const auto raw = static_cast<int32_t>(Random::Float() * static_cast<float>(count));
    // Random::Float() may legally return 1.0 → clamp the rounded value.
    return (raw < 0) ? 0 : (raw >= count ? count - 1 : raw);
}

// Pick one of the four corners and put the planet centre just outside it so
// only a thin wedge (peek * radius) shows in-view. The shader measures
// distance in aspect-corrected UV (toP.x *= aspectRatio), so a diagonal
// offset of (radius - peek) puts the corner exactly `peek` inside the disk.
glm::vec3 PickRandomPlanetCorner(const float planetRadiusUv, const float aspectRatio)
{
    constexpr float peekUv = 0.32f;
    constexpr float invSqrt2 = 0.70710678f;
    const float diag = (planetRadiusUv - peekUv) * invSqrt2;
    const float dxUv = diag / aspectRatio;
    const float dyUv = diag;
    const int32_t cornerId = RandomIndex(4); // 0=TL 1=TR 2=BL 3=BR
    switch (cornerId) {
    case 0:
        return glm::vec3(-dxUv, -dyUv, 0.0f);
    case 1:
        return glm::vec3(1.0f + dxUv, -dyUv, 0.0f);
    case 2:
        return glm::vec3(-dxUv, 1.0f + dyUv, 0.0f);
    case 3:
    default:
        return glm::vec3(1.0f + dxUv, 1.0f + dyUv, 0.0f);
    }
}

} // namespace

void AttachProceduralSpaceBackground(
    const std::shared_ptr<Scene>& scene, const std::shared_ptr<Actor>& targetActor, const SpaceBackgroundConfig& config)
{
    if (!scene || !targetActor) {
        return;
    }

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& material = materialParser.ParseMaterialDescriptor("SpaceBackgroundMaterial.m");
    scene->RegisterMaterialInstance(material);

    // Accumulating delta-time, bound from the scene clock.
    MaterialPropertySetter::SetMaterialPropertyValue(material, scene, "GT_DeltaSec", "gt_timeSec");

    const float aspectRatio = static_cast<float>(GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth())
        / static_cast<float>(GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight());
    MaterialPropertySetter::SetMaterialPropertyValue(material, "aspectRatio", aspectRatio);

    // Per-load offset that perturbs the volumetric starfield's sample origin
    // and the sparkle hash grid. Multiplied inside the shader by primes so a
    // small change to randomSeed produces a perceptibly different sky.
    MaterialPropertySetter::SetMaterialPropertyValue(material, "randomSeed", Random::Float());

    // Pick a colour palette for nebula + planet at random.
    const auto& palette = kPalettes[RandomIndex(static_cast<int32_t>(kPalettes.size()))];
    MaterialPropertySetter::SetMaterialPropertyValue(material, "nebulaHue", palette.nebulaA);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "nebulaTintB", palette.nebulaB);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "nebulaTintC", palette.nebulaC);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "planetHue", palette.planetHue);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "planetHighlight", palette.planetHighlight);

    // Planet placement + size. Skipped (in spirit) when includePlanet is false
    // — uniforms still need to be set to keep the GLSL contract simple, but
    // planetEnabled=0 short-circuits the body/halo/ring computation entirely.
    const float planetRadiusUv = config.includePlanet ? (0.32f + Random::Float() * 0.18f) : 0.30f;
    const glm::vec3 planetCenterUv
        = config.includePlanet ? PickRandomPlanetCorner(planetRadiusUv, aspectRatio) : glm::vec3(2.0f, 2.0f, 0.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "planetCenter", planetCenterUv);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "planetRadius", planetRadiusUv);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "planetEnabled", config.includePlanet ? 1 : 0);
    MaterialPropertySetter::SetMaterialPropertyValue(material, "ringEnabled", config.includeRing ? 1 : 0);

    // Fullscreen billboard — identity view/projection matrices coerce the
    // quad into clip space directly, so it covers the whole viewport no
    // matter where the camera is or how it moves.
    auto billboardCreator = std::make_shared<BillboardComponentCreator<BillboardComponent>>();
    const auto data = std::make_shared<BillboardComponentData>(
        "c_spaceBackgroundBillboard",
        1.0f, // billboardExtent
        true, // applyScreenAspectRatio
        glm::vec3(0.0f, 0.0f, 1.0f), // translation
        0.0f, // rotationRadians
        false, // isFlipped
        glm::vec3(1.0f), // scale
        material,
        [](const glm::mat4& /*viewMatrix*/) { return glm::mat4(1); }, // force-identity view
        [](const glm::mat4& /*projectionMatrix*/) { return glm::mat4(1); } // force-identity projection
    );
    const auto& billboard
        = std::static_pointer_cast<BillboardComponent>(scene->CreateComponent_GameThread(billboardCreator, data));
    billboard->SetSortOrderValue(-100000); // draw behind everything
    targetActor->AddComponent(billboard);
}

} // namespace Game
