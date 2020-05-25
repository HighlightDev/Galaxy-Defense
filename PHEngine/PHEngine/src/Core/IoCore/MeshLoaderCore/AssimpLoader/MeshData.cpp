#include "MeshData.h"
#include "SkeletonBoneLOADER.h"
#include "Core/UtilityCore/AssimpSkeletonConverter.h"
#include "Core/CommonCore/Assertion.h"

#include <tuple>
#include <thread>
#include <memory>

using namespace EngineUtility;

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{

         Collector::Collector(const aiScene* scene)
            : mScene(scene)
         {
         }

         void Collector::Collect()
         {
            if (!mScene)
               return;

            // Node structure
            {
               aiNode* rootNode = mScene->mRootNode;

               meshRootNode = new MeshNode();
               meshRootNode->Name = rootNode->mName.data;
               meshRootNode->NodeTransformation = AssimpSkeletonConverter::ConvertAssimpMatrix4x4ToGlmMat4(rootNode->mTransformation);
               MeshNodeMapping[meshRootNode->Name] = meshRootNode;

               CollectNodeHierarchy(rootNode, meshRootNode);
            }

            // Bones
            {
               CollectBones();
            }

            // Animations
            {
               CollectAnimation();
            }

            volatile int a = 5;
            a;
         }

         void Collector::CollectAnimation()
         {
            aiNode* rootNode = mScene->mRootNode;


            for (size_t i = 0; i < mScene->mNumAnimations; ++i)
            {
               const aiAnimation* pAnimation = mScene->mAnimations[i];

               std::string animationName(pAnimation->mName.data);
               AnimationMapping[animationName].AnimationDuration = (float)pAnimation->mDuration;

               AnimationIterateNodes(pAnimation, rootNode, AnimationMapping[animationName].NodeAnimationBindings);
            }
         }

         aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName);

         void Collector::AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings)
         {
            std::string nodeName(pNode->mName.data);

            aiNodeAnim* pNodeAnim = FindAnimationNodeByName(pAnimation, nodeName);

            if (pNodeAnim)
            {
               for (size_t i = 0; i < pNodeAnim->mNumRotationKeys; ++i)
               {
                  FrameRotation rotation;
                  rotation.Rotation = AssimpSkeletonConverter::ConvertAssimpQuatToGlmQuat(pNodeAnim->mRotationKeys[i].mValue);
                  rotation.Time = (float)pNodeAnim->mRotationKeys[i].mTime;
                  nodeAnimationBindings[nodeName].RotationFrames.emplace_back(std::move(rotation));
               }

               for (size_t i = 0; i < pNodeAnim->mNumPositionKeys; ++i)
               {
                  FrameTranslation translation;
                  translation.Translation = AssimpSkeletonConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mPositionKeys[i].mValue);
                  translation.Time = (float)pNodeAnim->mPositionKeys[i].mTime;
                  nodeAnimationBindings[nodeName].TranslationFrames.emplace_back(std::move(translation));
               }

               for (size_t i = 0; i < pNodeAnim->mNumScalingKeys; ++i)
               {
                  FrameScale scale;
                  scale.Scale = AssimpSkeletonConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mScalingKeys[i].mValue);
                  scale.Time = (float)pNodeAnim->mScalingKeys[i].mTime;
                  nodeAnimationBindings[nodeName].ScaleFrames.emplace_back(std::move(scale));
               }
            }

            for (size_t i = 0; i < pNode->mNumChildren; ++i) 
            {
               aiNode* child = pNode->mChildren[i];

               if (child)
               {
                  AnimationIterateNodes(pAnimation, child, nodeAnimationBindings);
               }
            }
         }

         aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName)
         {
            aiNodeAnim* result = nullptr;

            for (size_t i = 0; i < pAnimation->mNumChannels; ++i)
            {
               aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

               if (std::string(pNodeAnim->mNodeName.data) == nodeName)
               {
                  result = pNodeAnim;
                  break;
               }
            }

            return result;
         }

         void Collector::CollectBones()
         {
            size_t meshCount = mScene->mNumMeshes;

            for (size_t i = 0; i < meshCount; ++i)
            {
               aiMesh* mesh = mScene->mMeshes[i];

               for (size_t j = 0; j < mesh->mNumBones; ++j)
               {
                  aiBone* boneInfo = mesh->mBones[j];
                  MeshBoneInfo meshBoneInfo;
                  meshBoneInfo.BoneOffset = AssimpSkeletonConverter::ConvertAssimpMatrix4x4ToGlmMat4(boneInfo->mOffsetMatrix);
                  BoneMapping[std::string(boneInfo->mName.data)] = meshBoneInfo;
               }
            }
         }

         void Collector::CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode)
         {
            if (!pNode)
               return;

            for (size_t i = 0; i < pNode->mNumChildren; ++i)
            {
               aiNode* pChildNode = pNode->mChildren[i];
               MeshNode* meshChildNode = new MeshNode();
               meshChildNode->Name = pChildNode->mName.data;
               meshChildNode->NodeTransformation = AssimpSkeletonConverter::ConvertAssimpMatrix4x4ToGlmMat4(pChildNode->mTransformation);
               meshNode->Children.push_back(meshChildNode);
               MeshNodeMapping[meshChildNode->Name] = meshChildNode;
               
               CollectNodeHierarchy(pChildNode, meshChildNode);
            }
         }

         /************************************************************************/

         struct VertexBoneData
         {
            size_t BoneIds[4];
            float Weights[4];

            bool IsFilled = false;

            void AddBoneData(size_t boneId, float weight)
            {
               if (!IsFilled)
               {
                  for (size_t i = 0; i < 4; ++i)
                  {
                     if (Weights[i] <= 0.0005f)
                     {
                        BoneIds[i] = boneId;
                        Weights[i] = weight;
                        IsFilled = i == 3;
                        return;
                     }
                  }
               }
            }
         };

         struct NewBoneInfo
         {
            aiMatrix4x4 mBoneOffset;
         };

         struct Mesh {

            std::map<std::string, size_t> mBoneMap;
            std::vector<NewBoneInfo> mBoneInfo;

            size_t mNumBones = 0;

            void CollectBones(aiMesh* mesh, size_t meshVertexBaseIndex, std::vector<VertexBoneData>& bones)
            {
               for (size_t i = 0; i < mesh->mNumBones; i++) {
                  size_t BoneIndex = 0;
                  std::string BoneName(mesh->mBones[i]->mName.data);

                  if (mBoneMap.find(BoneName) == mBoneMap.end()) {
                     BoneIndex = mNumBones;
                     mNumBones++;
                     NewBoneInfo bi;
                     mBoneInfo.push_back(bi);
                  }
                  else {
                     BoneIndex = mBoneMap[BoneName];
                  }

                  mBoneMap[BoneName] = BoneIndex;
                  mBoneInfo[BoneIndex].mBoneOffset = mesh->mBones[i]->mOffsetMatrix;

                  for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
                     size_t VertexID = meshVertexBaseIndex + mesh->mBones[i]->mWeights[j].mVertexId;
                     float Weight = mesh->mBones[i]->mWeights[j].mWeight;
                     bones[VertexID].AddBoneData(BoneIndex, Weight);
                  }
               }
            }
         };

         void ReadNodeHeirarchy(const aiNode* pNode, std::set<std::string>& nodeNames)
         {
            nodeNames.insert(pNode->mName.data);

            for (size_t i = 0; i < pNode->mNumChildren; i++) {
               ReadNodeHeirarchy(pNode->mChildren[i], nodeNames);
            }
         }

         template <int32_t count_bones_influence_vertex>
         void MeshData<count_bones_influence_vertex>::NEW_CollectBoneInfo()
         {
            std::vector<Mesh> meshes;
            meshes.resize(m_scene->mNumMeshes);

            size_t countVertices = 0;

            std::vector<VertexBoneData> bones;
            for (size_t i = 0; i < m_scene->mNumMeshes; ++i)
            {
               countVertices += m_scene->mMeshes[i]->mNumVertices;
            }

            bones.resize(countVertices);

            size_t processedMeshBonesCount = 0;

            for (size_t i = 0; i < m_scene->mNumMeshes; ++i)
            {
               aiMesh* m = m_scene->mMeshes[i];
               
               Mesh meshData;
               meshData.CollectBones(m, processedMeshBonesCount, bones);
               meshes.push_back(meshData);
               processedMeshBonesCount += m->mNumVertices;
            }

            std::set<std::string> nodeNames;

            ReadNodeHeirarchy(m_scene->mRootNode, nodeNames);
         }

			template struct MeshData<3>;

			template <int32_t count_bones_influence_vertex>
			MeshData<count_bones_influence_vertex>::MeshData(const aiScene* scene)
				: m_scene(scene)
				, m_meshes(m_scene->mMeshes)
				, SkeletonRoot(nullptr)
			{
				GetMeshData();
			}

			template <int32_t count_bones_influence_vertex>
			MeshData<count_bones_influence_vertex>::~MeshData()
			{
            delete SkeletonRoot;
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::GetMeshData()
			{
				size_t meshCount = m_scene->mNumMeshes;

				for (size_t currentMeshIndex = size_t(0); currentMeshIndex < meshCount; currentMeshIndex++)
				{
					if (!bHasIndices)
						bHasIndices = m_meshes[currentMeshIndex]->HasFaces();
					if (!bHasNormals)
						bHasNormals = m_meshes[currentMeshIndex]->HasNormals();
					if (!bHasTextureCoordinates)
						bHasTextureCoordinates = m_meshes[currentMeshIndex]->HasTextureCoords(0);
               if (!bHasTangentVertices)
                  bHasTangentVertices = m_meshes[currentMeshIndex]->HasTangentsAndBitangents();

					// If we have all necessary info already -> stop passing through array
					if (bHasIndices && bHasNormals && bHasTextureCoordinates && bHasTangentVertices)
						break;
				}

				bHasAnimation = m_scene->HasAnimations();

            NEW_CollectBoneInfo();

            Collector collector(m_scene);
            collector.Collect();

				LoadSkeleton();
				LoadSkin();

			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::LoadSkin() {

				size_t countOfVertices = 0;
				size_t meshCount = m_scene->mNumMeshes;
				for (size_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
				{
					aiMesh* mesh = m_scene->mMeshes[meshIndex];
					countOfVertices += mesh->mNumVertices;
				}

				std::vector<uint32_t> countOfIndicesPerMesh;

				// Iterate through all meshes
				for (size_t i = 0; i < m_scene->mNumMeshes; i++)
				{
					aiMesh* mesh = m_meshes[i];
					size_t processedIndices = Indices.size();
					countOfIndicesPerMesh.emplace_back(processedIndices);
					CollectIndices(mesh, processedIndices);
					TryToCollectSkinInfo(countOfIndicesPerMesh[i], mesh);
				}

				// collect blend weights and blend id's
				if (bHasAnimation)
				{
					std::vector<VertexLOADER> blendData;

               // Collect blend data concurrently, if possible
               const size_t countOfAvailableThreads = std::thread::hardware_concurrency();
               size_t countOfVerticesPerJob = countOfVertices / countOfAvailableThreads;

               auto jobFunctor = [&](const size_t startIndex, const size_t endIndex, std::vector<VertexLOADER>& collectionOfBlendables) {

                  for (size_t attributeIndex = startIndex; attributeIndex < endIndex; attributeIndex++)
                  {
                     for (size_t meshIndex = 0; meshIndex < m_scene->mNumMeshes; meshIndex++)
                     {
                        aiMesh* mesh = m_meshes[meshIndex];
                        const size_t seekVertexId = (attributeIndex + countOfIndicesPerMesh[meshIndex]);
                        CollectBlendables(seekVertexId, collectionOfBlendables, mesh);
                     }
                  }
               };

               std::vector<VertexLOADER>* jobBlendables = new std::vector<VertexLOADER>[countOfAvailableThreads];
               std::vector<std::shared_ptr<std::thread>> jobs;

               for (size_t jobIndex = 0; jobIndex < countOfAvailableThreads; jobIndex++)
               {
                  const size_t startIndex = jobIndex * countOfVerticesPerJob;
                  const size_t endIndex = jobIndex == (countOfAvailableThreads - 1) ? countOfVertices : (jobIndex + 1) * countOfVerticesPerJob;

                  std::vector<VertexLOADER>& currentJobVector = jobBlendables[jobIndex];
                  std::shared_ptr<std::thread> job = std::make_shared<std::thread>(jobFunctor, startIndex, endIndex, std::ref<std::vector<VertexLOADER>>(currentJobVector));
                  jobs.push_back(job);
               }

               for (auto& job : jobs)
               {
                  job->join();
               }

               for (size_t  index = 0; index < countOfAvailableThreads; index++)
               {
                 std::vector<VertexLOADER>& blendVector = jobBlendables[index];
                  blendData.insert(blendData.end(), std::make_move_iterator(blendVector.begin()), std::make_move_iterator(blendVector.end()));
               }

               delete[] jobBlendables;

					for (size_t blendableIndex = 0; blendableIndex < blendData.size(); blendableIndex++)
					{
						VertexLOADER& blendVertex = blendData[blendableIndex];
						CollectBlendWeightsAndIndices(blendVertex, blendableIndex);
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::LoadSkeleton() {

				if (bHasAnimation)
				{
					SkeletonRoot = new SkeletonBoneBaseLOADER();
					int32_t boneIdCounter = 0;

					aiNode* rootNode = m_scene->mRootNode->FindNode(GetSkeletonArmatureNodeName(*m_scene->mRootNode));
					if (rootNode)
					{
                  const aiMatrix4x4 invertedGlobalTransform = rootNode->mTransformation.Inverse();

                  if (invertedGlobalTransform.a1 != std::numeric_limits<float>::quiet_NaN())
                  {
                     SkeletonRoot->InvGlobaTransform = invertedGlobalTransform;
                  }

						size_t childrenCount = rootNode->mNumChildren;
						for (size_t childIndex = 0; childIndex < childrenCount; ++childIndex)
						{
							aiNode* childNode = rootNode->mChildren[childIndex];

                     NodeNames.insert(childNode->mName.data);

							aiBone* bone = GetBoneByName(childNode->mName);
							if (bone)
							{
								SkeletonBoneLOADER* skeletonBone = new SkeletonBoneLOADER(SkeletonRoot);
								skeletonBone->SetBoneId(boneIdCounter);
                        skeletonBone->SetBoneInfo(AssimpSkeletonConverter::ConvertAssimpBoneInfoToEngineBoneInfo(bone));
                        BoneMaping[bone->mName.data] = boneIdCounter;
                        boneIdCounter++;
								
								FillHierarchyRecursive(childNode, skeletonBone, boneIdCounter);
								SkeletonRoot->AddChildBone(skeletonBone);
							}
						}
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::FillHierarchyRecursive(aiNode* parentNode, SkeletonBoneLOADER*& parentBone, int32_t& boneIdCounter) {

				size_t nodesCount = parentNode->mNumChildren;
				for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
				{
					aiNode* childNode = parentNode->mChildren[nodeIndex];

               NodeNames.insert(childNode->mName.data);

               aiBone* bone = GetBoneByName(childNode->mName);

               SkeletonBoneLOADER* childBone = parentBone;
               if (bone)
               {
                  childBone = new SkeletonBoneLOADER(parentBone);
                  parentBone->AddChildBone(childBone);
                  childBone->SetBoneInfo(AssimpSkeletonConverter::ConvertAssimpBoneInfoToEngineBoneInfo(bone));
                  childBone->SetBoneId(boneIdCounter);
                  BoneMaping[bone->mName.data] = boneIdCounter;
                  boneIdCounter++;
               }

					FillHierarchyRecursive(childNode, childBone, boneIdCounter);
				}
			}

			template <int32_t count_bones_influence_vertex>
			aiString MeshData<count_bones_influence_vertex>::GetSkeletonArmatureNodeName(aiNode& rootNode) {

            aiString boneName;

				int32_t maxHierarchySize = 0;
				size_t nodesCount = rootNode.mNumChildren;

				for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
				{
					aiNode* childNode = rootNode.mChildren[nodeIndex];
					int32_t currentChildHierarchySize = 0;
					IterateHierarchy(*childNode, currentChildHierarchySize);
					if (currentChildHierarchySize > maxHierarchySize)
					{
						maxHierarchySize = currentChildHierarchySize;
                  boneName = childNode->mName;
					}
				}

				return boneName;
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::IterateHierarchy(aiNode& parentNode, int32_t& countChildren) {

				size_t nodesCount = parentNode.mNumChildren;
				for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
				{
					aiNode& childNode = *parentNode.mChildren[nodeIndex];
					++countChildren;
					IterateHierarchy(childNode, countChildren);
				}
			}

			template <int32_t count_bones_influence_vertex>
			aiBone* MeshData<count_bones_influence_vertex>::GetBoneByName(const aiString& name) const {

				aiBone* result = nullptr;

				size_t meshCount = m_scene->mNumMeshes;

				for (size_t currentMeshIndex = size_t(0); (currentMeshIndex < meshCount) && (result == nullptr); currentMeshIndex++)
				{
					aiMesh* mesh = m_meshes[currentMeshIndex];
					size_t bonesCount = mesh->mNumBones;
					for (size_t boneIndex = size_t(0); boneIndex < bonesCount; boneIndex++)
					{
						aiBone* bone = mesh->mBones[boneIndex];
						if (bone->mName == name)
						{
							result = bone;
							break;
						}
					}
				}
				return result;
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::CollectIndices(aiMesh* meshBeingCollected, uint32_t lastIndexBeenInterrupted) {

				size_t countOfFaces = meshBeingCollected->mNumFaces;
				for (size_t faceIndex = 0; faceIndex < countOfFaces; faceIndex++)
				{
					aiFace& face = meshBeingCollected->mFaces[faceIndex];
					if (face.mNumIndices == 3) // triangulated face
					{
						Indices.emplace_back(face.mIndices[0] + lastIndexBeenInterrupted);
						Indices.emplace_back(face.mIndices[1] + lastIndexBeenInterrupted);
						Indices.emplace_back(face.mIndices[2] + lastIndexBeenInterrupted);
					}
					else
					{
						throw std::invalid_argument("Face isn't triangulated.");
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::TryToCollectSkinInfo(size_t startIndex, aiMesh* meshBeingCollected) {

            const bool bCollectNormals = meshBeingCollected->HasNormals() & bHasNormals;
            const bool bCollectTexCoords = meshBeingCollected->HasTextureCoords(0) & bHasTextureCoordinates;
            const bool bCollectTangBitang = meshBeingCollected->HasTangentsAndBitangents() & bHasTangentVertices;

				for (size_t attribIndex = 0; attribIndex < meshBeingCollected->mNumVertices; ++attribIndex)
				{
					Verts.emplace_back(meshBeingCollected->mVertices[attribIndex].x);
					Verts.emplace_back(meshBeingCollected->mVertices[attribIndex].y);
					Verts.emplace_back(meshBeingCollected->mVertices[attribIndex].z);

					if (bCollectNormals)
					{
						N_Verts.emplace_back(meshBeingCollected->mNormals[attribIndex].x);
						N_Verts.emplace_back(meshBeingCollected->mNormals[attribIndex].y);
						N_Verts.emplace_back(meshBeingCollected->mNormals[attribIndex].z);
					}
					if (bCollectTexCoords)
					{
						T_Verts.emplace_back(meshBeingCollected->mTextureCoords[0][attribIndex].x);
						T_Verts.emplace_back(meshBeingCollected->mTextureCoords[0][attribIndex].y);
					}
					if (bCollectTangBitang)
					{
						Tangent_Verts.emplace_back(meshBeingCollected->mTangents[attribIndex].x);
						Tangent_Verts.emplace_back(meshBeingCollected->mTangents[attribIndex].y);
						Tangent_Verts.emplace_back(meshBeingCollected->mTangents[attribIndex].z);

						Bitanget_Verts.emplace_back(meshBeingCollected->mBitangents[attribIndex].x);
						Bitanget_Verts.emplace_back(meshBeingCollected->mBitangents[attribIndex].y);
						Bitanget_Verts.emplace_back(meshBeingCollected->mBitangents[attribIndex].z);
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::CollectBlendables(size_t vertexId, std::vector<VertexLOADER>& blendData, aiMesh* meshBeingCollected) {

				VertexLOADER vertex(vertexId);

				aiBone** bonesInMesh = meshBeingCollected->mBones;
				size_t bonesCount = meshBeingCollected->mNumBones;

				for (size_t boneIndex = 0; boneIndex < bonesCount; boneIndex++)
				{
					aiBone* bone = bonesInMesh[boneIndex];
					size_t weightsCount = bone->mNumWeights;

					for (size_t weightIndex = 0; weightIndex < weightsCount; weightIndex++)
					{
						aiVertexWeight& weight = bone->mWeights[weightIndex];

                  if (weight.mVertexId > vertexId) // skip iteration in case if current vertex id is too big
                     weightIndex = weight.mVertexId;

						if (weight.mVertexId == vertexId)
						{
							int32_t boneId = SkeletonRoot->GetIdByBoneInHierarchy(bone);
							if (boneId >= 0)
							{
								std::tuple<aiBone*, int32_t> weightTuple = std::tuple<aiBone*, int32_t>(bone, boneId);
								std::pair<std::tuple<aiBone*, int32_t>, float> weightPair = std::make_pair<std::tuple<aiBone*, int32_t>, float>(std::move(weightTuple), std::move(weight.mWeight));
								vertex.AddBoneWeight(std::move(weightPair));
							}
							else
							{
								//throw std::invalid_argument("Such bone doesn't exist in skeleton!");
							}
						}
					}
				}

				if (vertex.BoneWeightMap.size() > 0)
               blendData.emplace_back(std::move(vertex));
			}

			template <>
			void MeshData<3>::CollectBlendWeightsAndIndices(VertexLOADER& blendInfoVertex, size_t blendableIndex)
			{
				const size_t currentVertexInfluenceCount = blendInfoVertex.BoneWeightMap.size();

				if (currentVertexInfluenceCount == 1)
				{
					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights.emplace_back(0.0f);
					BlendIndices.emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.

					BlendWeights.emplace_back(0.0f);
					BlendIndices.emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.
				}
				else if (currentVertexInfluenceCount == 2)
				{
					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[1].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[1].first));

					BlendWeights.emplace_back(0.0f);
					BlendIndices.emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.
				}
				else if (currentVertexInfluenceCount > 2)
				{
					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[1].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[1].first));

					BlendWeights.emplace_back(blendInfoVertex.BoneWeightMap[2].second);
					BlendIndices.emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[2].first));
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshData<count_bones_influence_vertex>::CleanUp()
			{

			}

         template <int32_t count_bones_influence_vertex>
         std::map<std::string, size_t> MeshData<count_bones_influence_vertex>::GetValidBoneMapping() const {
            return BoneMaping;
         }
		}
	}
}