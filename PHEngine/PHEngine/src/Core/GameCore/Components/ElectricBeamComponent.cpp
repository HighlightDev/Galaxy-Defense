#include "ElectricBeamComponent.h"

#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/LoggerExtension.h"

#include <random>

namespace EngineCore {

ElectricBeamComponent::ElectricBeamComponent(const std::string& gameObjectName)
    : PrimitiveComponent(gameObjectName, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f))
    , mBeamMeshes()
    , mStartPoint(glm::vec3(0.0f))
    , mEndPoint(glm::vec3(10.0f, 0.0f, 0.0f))
    , mBeamColor(glm::vec3(0.3f, 0.5f, 1.0f))
    , mBeamThickness(2.0f)
    , mBeamCount(3)
    , mJitterAmount(0.2f)
    , mUpdateFrequency(0.05f)
    , mIsActive(true)
    , mRenderMode(BeamRenderMode::ProceduralMesh)
    , mRadialSegments(8)
    , mLengthSegments(10)
    , mAnimationTime(0.0f)
    , mAnimationSpeed(2.0f)
    , mTimeSinceLastUpdate(0.0f)
{
}

ElectricBeamComponent::~ElectricBeamComponent()
{
    DestroyBeamMeshes();
}

void ElectricBeamComponent::Tick(const float deltaTime)
{
    Component::Tick(deltaTime);

    if (!mIsActive) {
        return;
    }

    // Update animation time continuously for smooth procedural animation
    mAnimationTime += deltaTime;

    mTimeSinceLastUpdate += deltaTime;

    if (mTimeSinceLastUpdate >= mUpdateFrequency) {
        UpdateBeamPositions();
        mTimeSinceLastUpdate = 0.0f;
    }
}

void ElectricBeamComponent::Initialize()
{
    LogInfo("ElectricBeamComponent::Initialize: ", GetEngineObjectName());

    CreateBeamMeshes();

    RegenerateBeams();
}

void ElectricBeamComponent::SetStartPoint(const glm::vec3& point)
{
    mStartPoint = point;
    RegenerateBeams();
}

void ElectricBeamComponent::SetEndPoint(const glm::vec3& point)
{
    mEndPoint = point;
    RegenerateBeams();
}

void ElectricBeamComponent::SetBeamColor(const glm::vec3& color)
{
    mBeamColor = color;
}

void ElectricBeamComponent::SetBeamThickness(float thickness)
{
    mBeamThickness = thickness;
}

void ElectricBeamComponent::SetBeamCount(int count)
{
    if (count < 1)
        count = 1;
    if (count > 20)
        count = 20; // Reasonable limit

    mBeamCount = count;

    DestroyBeamMeshes();
    CreateBeamMeshes();

    RegenerateBeams();
}

void ElectricBeamComponent::SetJitterAmount(float amount)
{
    mJitterAmount = amount;
}

void ElectricBeamComponent::SetUpdateFrequency(float frequency)
{
    if (frequency < 0.01f)
        frequency = 0.01f;
    mUpdateFrequency = frequency;
}

void ElectricBeamComponent::SetActive(bool active)
{
    mIsActive = active;

    for (auto& mesh : mBeamMeshes) {
        if (mesh) {
            mesh->SetIsVisible(active);
        }
    }
}

glm::vec3 ElectricBeamComponent::GetStartPoint() const
{
    return mStartPoint;
}

glm::vec3 ElectricBeamComponent::GetEndPoint() const
{
    return mEndPoint;
}

glm::vec3 ElectricBeamComponent::GetBeamColor() const
{
    return mBeamColor;
}

float ElectricBeamComponent::GetBeamThickness() const
{
    return mBeamThickness;
}

int ElectricBeamComponent::GetBeamCount() const
{
    return mBeamCount;
}

float ElectricBeamComponent::GetJitterAmount() const
{
    return mJitterAmount;
}

float ElectricBeamComponent::GetUpdateFrequency() const
{
    return mUpdateFrequency;
}

bool ElectricBeamComponent::IsActive() const
{
    return mIsActive;
}

void ElectricBeamComponent::RegenerateBeams()
{

    for (int i = 0; i < static_cast<int>(mBeamMeshes.size()); ++i) {
        UpdateBeamMesh(i);
    }
}

void ElectricBeamComponent::UpdateBeamPositions()
{
    RegenerateBeams();
}

glm::vec3 ElectricBeamComponent::GetJitteredPoint(const glm::vec3& basePoint, float jitterScale) const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    const float jitter = mJitterAmount * jitterScale;

    return basePoint + glm::vec3(dis(gen) * jitter, dis(gen) * jitter, dis(gen) * jitter);
}

void ElectricBeamComponent::SetRenderMode(BeamRenderMode mode)
{
    if (mRenderMode == mode) {
        return;
    }

    // Clean up old rendering method
    DestroyBeamMeshes();

    mRenderMode = mode;

    // Create new rendering method
    CreateBeamMeshes();

    RegenerateBeams();

    LogInfo("ElectricBeamComponent::SetRenderMode: Changed to ", static_cast<int>(mode));
}

void ElectricBeamComponent::SetGeometrySegments(int radialSegments, int lengthSegments)
{
    if (radialSegments < 3)
        radialSegments = 3;
    if (lengthSegments < 2)
        lengthSegments = 2;

    mRadialSegments = radialSegments;

    mLengthSegments = lengthSegments;
    // Recreate meshes with new segment count
    DestroyBeamMeshes();
    CreateBeamMeshes();
    RegenerateBeams();
}

BeamRenderMode ElectricBeamComponent::GetRenderMode() const
{
    return mRenderMode;
}

void ElectricBeamComponent::SetAnimationSpeed(float speed)
{
    if (speed < 0.0f)
        speed = 0.0f;
    mAnimationSpeed = speed;
    LogInfo("ElectricBeamComponent::SetAnimationSpeed: ", speed);
}

float ElectricBeamComponent::GetAnimationSpeed() const
{
    return mAnimationSpeed;
}

void ElectricBeamComponent::CreateBeamMeshes()
{
    const int beamCount = mBeamCount;
    const bool isActive = mIsActive;

    mBeamMeshes.clear();
    mBeamMeshes.reserve(beamCount);

    for (int i = 0; i < beamCount; ++i) {
        auto mesh = std::make_shared<StaticMeshComponent>(GetEngineObjectName() + "_BeamMesh_" + std::to_string(i));

        mesh->SetVisible(isActive);

        mBeamMeshes.push_back(mesh);
    }

    LogInfo("ElectricBeamComponent::CreateBeamMeshes: Created ", beamCount, " beam meshes");
}

void ElectricBeamComponent::DestroyBeamMeshes()
{
    mBeamMeshes.clear();
    LogInfo("ElectricBeamComponent::DestroyBeamMeshes: Destroyed all beam meshes");
}

void ElectricBeamComponent::UpdateBeamMesh(int beamIndex)
{
    if (beamIndex < 0 || beamIndex >= static_cast<int>(mBeamMeshes.size())) {
        return;
    }

    auto& mesh = mBeamMeshes[beamIndex];
    if (!mesh) {
        return;
    }

    const glm::vec3 startPoint = mStartPoint;
    const glm::vec3 endPoint = mEndPoint;
    const float radius = mBeamThickness * 0.1f; // Convert thickness to radius

    std::vector<BeamVertex> vertices;
    std::vector<uint32_t> indices;

    if (mRenderMode == BeamRenderMode::ProceduralMesh) {
        // Simple cylindrical beam with animated jitter
        glm::vec3 jitteredStart = GetJitteredPoint(startPoint, 0.3f);
        glm::vec3 jitteredEnd = GetJitteredPoint(endPoint, 0.3f);

        ProceduralBeamGeometry::GenerateBeamGeometry(jitteredStart, jitteredEnd, radius, mRadialSegments, vertices, indices);
    } else if (mRenderMode == BeamRenderMode::ProceduralElectric) {
        // Electric beam with animated jittered segments
        glm::vec3 jitteredStart = GetJitteredPoint(startPoint, 0.2f);
        glm::vec3 jitteredEnd = GetJitteredPoint(endPoint, 0.2f);

        // Use animated version with continuous time parameter
        ProceduralBeamGeometry::GenerateAnimatedElectricBeamGeometry(
            jitteredStart,
            jitteredEnd,
            radius,
            mRadialSegments,
            mLengthSegments,
            mJitterAmount,
            mAnimationTime + beamIndex * 0.5f, // Offset animation time per beam
            mAnimationSpeed,
            vertices,
            indices);
    }

    // TODO: Update mesh geometry
    // This would require integration with the mesh system to update vertex/index buffers
    // For now, just log the vertex count
    LogInfo(
        "ElectricBeamComponent::UpdateBeamMesh[",
        beamIndex,
        "]: Generated ",
        vertices.size(),
        " vertices, ",
        indices.size(),
        " indices (time: ",
        mAnimationTime,
        ")");
}

} // namespace EngineCore
