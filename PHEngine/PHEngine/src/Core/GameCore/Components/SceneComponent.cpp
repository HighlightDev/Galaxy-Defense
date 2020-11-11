#include "SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;

namespace Game
{

   SceneComponent::SceneComponent(const std::string& gameObjectName)
      : Component(gameObjectName)
      , bTransformationDirty(true)
      , mIsVisible(GenericObjectProperty<bool>(true, "IsVisible"))
      , mTransform(std::make_shared<Transform>())
      , m_additionalRotationEuler()
      , m_relativeMatrix(1)
      , m_scene(nullptr)
   {
      /******  HOOKS ****/
      ENGINE_PROPERTY("IsVisible", &mIsVisible);
      /******  HOOKS ****/
   }

	SceneComponent::SceneComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		: Component(gameObjectName)
      , bTransformationDirty(true)
      , mIsVisible(GenericObjectProperty<bool>(true, "IsVisible"))
      , mTransform(std::make_shared<Transform>(translation, glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z))), scale))
      , m_additionalRotationEuler()
		, m_relativeMatrix(std::move(glm::mat4(1)))
      , m_scene(nullptr)
	{
      /******  HOOKS ****/
      ENGINE_PROPERTY("IsVisible", &mIsVisible);
      /******  HOOKS ****/
	}

	SceneComponent::~SceneComponent()
	{
	}

	void SceneComponent::Tick(const float deltaTime)
	{
	}

   void SceneComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
   }

   ComponentType SceneComponent::GetComponentType() const
   {
      return SCENE_COMPONENT;
   }

   void SceneComponent::AddOffsetUp(const float offsetValue)
   {
      mTransform->Translation = mTransform->Translation + AXIS_UP * offsetValue;
   }

   void SceneComponent::SetIsVisible(bool isVisible)
   {
      if (isVisible != mIsVisible)
      {
         mIsVisible = isVisible;
         OnVisibilityChanged();
      }
   }
 
	void SceneComponent::UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix)
	{
		// Update current relative matrix

      glm::mat4 identityMatrix(1);
      m_relativeMatrix = identityMatrix;

	   glm::mat4 translationMatrix = glm::translate(identityMatrix, mTransform->Translation);

      m_relativeMatrix *= parentRelativeMatrix;
      m_relativeMatrix *= glm::scale(identityMatrix, mTransform->Scale);
      m_relativeMatrix *= translationMatrix;

      if (bIsRootComponent)
      {
         glm::mat4 cameraYawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(m_additionalRotationEuler.y), AXIS_UP);
         m_relativeMatrix *= cameraYawRotation;     
      }

      glm::mat4 rotationMatrix = glm::toMat4(mTransform->Rotator);
      m_relativeMatrix *= rotationMatrix;

		SetIsTransformationDirty(false);
	}

   bool SceneComponent::IsVisible() const {
      return mIsVisible;
   }

   void SceneComponent::OnVisibilityChanged() 
   {

   }

   void SceneComponent::SetScene(class Scene* scene)
   {
      m_scene = scene;
   }

   void SceneComponent::SetIsTransformationDirty(const bool isDirty)
   {
      bTransformationDirty = true;
   }

   void SceneComponent::SetTranslation(const glm::vec3& translation)
   {
      mTransform->Translation = translation;
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetRotator(const glm::quat& rotator)
   {
      mTransform->Rotator = rotator;
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetScale(glm::vec3 scale, const bool bTriggerTransformUpdateEvent)
   {
      mTransform->Scale = scale;
      SetIsTransformationDirty(true);
   }

   void SceneComponent::SetAdditionalRotation(const glm::vec3& rotationEuler, const bool bTriggerTransformUpdateEvent)
   {
      m_additionalRotationEuler = rotationEuler;
      SetIsTransformationDirty(true);
   }

   std::weak_ptr<Transform> SceneComponent::GetTransformWeakPtr() const {
      return mTransform;
   }

   bool SceneComponent::GetIsTransformationDirty() const {
      return bTransformationDirty;
   }

   glm::vec3 SceneComponent::GetTranslation() const
   {
      return mTransform->Translation;
   }

   glm::quat SceneComponent::GetRotator() const
   {
      return mTransform->Rotator;
   }

   glm::vec3 SceneComponent::GetRotationEuler() const
   {
      constexpr float radToDeg = 180.f / 3.14159f;
      return glm::eulerAngles(GetRotator()) * radToDeg;
   }

   glm::vec3 SceneComponent::GetScale() const
   {
      return mTransform->Scale;
   }

   glm::mat4 SceneComponent::GetRelativeMatrix() const
   {
      return m_relativeMatrix;
   }

}
