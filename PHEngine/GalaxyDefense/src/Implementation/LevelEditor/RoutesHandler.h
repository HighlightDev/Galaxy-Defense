#pragma once

#include <stdint.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <tuple>

namespace EngineCore
{
    class RuntimeGeneratedQuadraticBezierCurveComponent;
    class Scene;
    class Actor;
}

namespace Graphics
{
    class IMaterial;
}

namespace Game
{
    class RoutesHandler
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<::EngineCore::Actor> mBezierCurvesActor;

        int32_t mCurveComponentCounter{0};

        std::string mCurrentActiveRouteName;

        glm::vec3 mNewBezierCurveColor;

        std::unordered_map<std::string, std::stack<std::shared_ptr<::EngineCore::RuntimeGeneratedQuadraticBezierCurveComponent>>> mActiveRoutes;

        std::stack<std::shared_ptr<::EngineCore::RuntimeGeneratedQuadraticBezierCurveComponent>> mIdleCurveComponents;

        std::vector<glm::vec3> mPotentialBezierCurveControlPoints;

    public:
        explicit RoutesHandler(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                               const std::shared_ptr<::EngineCore::Actor> &bezierCurvesActor);

        void AddPointToActiveRoute(const std::shared_ptr<::Graphics::IMaterial> &splineMaterialPrefab, const glm::vec3 &newPoint);

        void UndoLastBezierCurveComponent();

        std::unordered_map<std::string, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> CollectRoutesControlPoints() const;

        void SelectNewRouteAsActive(const std::string &newRouteName);

        void SetNewBezierCurveColor(const glm::vec3 &color);

    private:
        void SpawnBezierCurveComponent(const std::shared_ptr<::Graphics::IMaterial> &splineMaterialPrefab,
                                       const glm::vec3 &pointA,
                                       const glm::vec3 &controlPoint,
                                       const glm::vec3 &pointB);
    };
}
