#include "RoutesHandler.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"

using namespace EngineCore;
using namespace Graphics;

namespace Game {
RoutesHandler::RoutesHandler(const std::weak_ptr<Scene>& sceneWp, const std::shared_ptr<Actor>& bezierCurvesActor)
    : mSceneWp(sceneWp)
    , mBezierCurvesActor(bezierCurvesActor)
{
    mPotentialBezierCurveControlPoints.reserve(3);
}

void RoutesHandler::SetNewBezierCurveColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mNewBezierCurveColor)) {
        mNewBezierCurveColor = color;
    }
}

void RoutesHandler::SpawnBezierCurveComponent(
    const std::shared_ptr<IMaterial>& splineMaterialPrefab,
    const glm::vec3& pointA,
    const glm::vec3& controlPoint,
    const glm::vec3& pointB)
{
    ext_assert(mCurrentActiveRouteName != "", "RoutesHandler current active route name is empty");
    const auto& sceneSp = mSceneWp.lock();
    std::shared_ptr<IMaterial> newSplineMaterial = splineMaterialPrefab;
    if (!mActiveRoutes.count(mCurrentActiveRouteName)) {
        MaterialParser materialParser;
        newSplineMaterial = materialParser.ParseMaterialDescriptor("CurveLineMaterial.m");
        MaterialPropertySetter::SetMaterialPropertyValue(newSplineMaterial, "opacity", 1.0f);
        MaterialPropertySetter::SetMaterialPropertyValue(newSplineMaterial, "color", mNewBezierCurveColor);
        sceneSp->RegisterMaterialInstance(newSplineMaterial);
    } else {
        newSplineMaterial = mActiveRoutes.at(mCurrentActiveRouteName).top()->GetMaterial();
    }

    if (mIdleCurveComponents.empty()) {
        const auto& d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
            "c_bezierCurveLineMesh_" + std::to_string(mCurveComponentCounter++),
            150,
            glm::vec3(0, 5, 0),
            glm::vec3(),
            glm::vec3(1),
            newSplineMaterial);
        const auto& meshComponentCreator
            = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedQuadraticBezierCurveComponent>>();
        auto c_mesh = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveComponent>(
            sceneSp->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        c_mesh->SetLineWidth(0.75f);
        c_mesh->SetSortOrderValue(30);
        c_mesh->SetCurveSegmentsCount(50);
        c_mesh->SetLineBeginWorldSpacePosition(pointA);
        c_mesh->SetBezierControlPointWorldSpacePosition(controlPoint);
        c_mesh->SetLineEndWorldSpacePosition(pointB);
        mBezierCurvesActor->AddComponent(c_mesh);
        mActiveRoutes[mCurrentActiveRouteName].emplace(c_mesh);
    } else {
        const auto& idleCurveComponent = mIdleCurveComponents.top();
        idleCurveComponent->SetIsEnabled(true);
        idleCurveComponent->SetLineBeginWorldSpacePosition(pointA);
        idleCurveComponent->SetBezierControlPointWorldSpacePosition(controlPoint);
        idleCurveComponent->SetLineEndWorldSpacePosition(pointB);
        mActiveRoutes[mCurrentActiveRouteName].emplace(idleCurveComponent);
        mIdleCurveComponents.pop();
    }
}

void RoutesHandler::AddPointToActiveRoute(const std::shared_ptr<IMaterial>& splineMaterialPrefab, const glm::vec3& newPoint)
{
    if (mCurrentActiveRouteName == "") {
        return;
    }

    const bool bNewPointNotConnectedWithPrevious
        = (!mActiveRoutes.size() || !mActiveRoutes.count(mCurrentActiveRouteName) || mPotentialBezierCurveControlPoints.size());
    if (bNewPointNotConnectedWithPrevious) {
        mPotentialBezierCurveControlPoints.emplace_back(newPoint);
    } else {
        ext_assert(mActiveRoutes.count(mCurrentActiveRouteName), "Missing route with name: " + mCurrentActiveRouteName);
        const auto& previousLineEndPosition = mActiveRoutes.at(mCurrentActiveRouteName).top()->GetLineEndWorldSpacePosition();
        if (EngineMath::CheckSimilarityVec3(previousLineEndPosition, newPoint)) {
            // In the same route all bezier curves have to be connected
            mPotentialBezierCurveControlPoints.emplace_back(newPoint);
        }
    }

    if (mPotentialBezierCurveControlPoints.size() >= 3) {
        SpawnBezierCurveComponent(
            splineMaterialPrefab,
            mPotentialBezierCurveControlPoints.at(0),
            mPotentialBezierCurveControlPoints.at(1),
            mPotentialBezierCurveControlPoints.at(2));
        mPotentialBezierCurveControlPoints.clear();
    }
}

void RoutesHandler::UndoLastBezierCurveComponent()
{
    if (mCurrentActiveRouteName != "" && mActiveRoutes.count(mCurrentActiveRouteName)
        && !mActiveRoutes.at(mCurrentActiveRouteName).empty()) {
        const auto& lastActiveCurveComponent = mActiveRoutes.at(mCurrentActiveRouteName).top();
        lastActiveCurveComponent->SetIsEnabled(false);
        mIdleCurveComponents.emplace(lastActiveCurveComponent);
        mActiveRoutes.at(mCurrentActiveRouteName).pop();
    }
}

std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>>
RoutesHandler::CollectRoutesControlPoints() const
{
    if (mActiveRoutes.size()) {
        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> routesResultMap;

        for (const auto& [routeName, route] : mActiveRoutes) {
            std::vector<std::tuple<glm::vec3 /*start*/, glm::vec3 /*control point*/, glm::vec3 /*end*/>> routePathControlPoints;
            routePathControlPoints.reserve(route.size() * 3);
            auto curveComponentsCopy = route;
            while (!curveComponentsCopy.empty()) {
                const auto& topPathComponent = curveComponentsCopy.top();
                curveComponentsCopy.pop();
                routePathControlPoints.emplace_back(std::make_tuple<glm::vec3, glm::vec3, glm::vec3>(
                    topPathComponent->GetLineBeginWorldSpacePosition(),
                    topPathComponent->GetBezierControlPointWorldSpacePosition(),
                    topPathComponent->GetLineEndWorldSpacePosition()));
                std::rotate(routePathControlPoints.rbegin(), routePathControlPoints.rbegin() + 1, routePathControlPoints.rend());
            }
            routesResultMap.emplace(routeName, routePathControlPoints);
        }
        return routesResultMap;
    }
    return {};
}

void RoutesHandler::SelectNewRouteAsActive(const std::string& newRouteName)
{
    ext_assert(!mActiveRoutes.count(newRouteName), "Route with name " + newRouteName + " already exists.");
    mCurrentActiveRouteName = newRouteName;
    mPotentialBezierCurveControlPoints.clear();
}
} // namespace Game
