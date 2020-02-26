#include "MeshVertexData.h"
#include "SkeletonBoneLOADER.h"

#include <tuple>
#include <thread>

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			template struct MeshVertexData<3>;

			template <int32_t count_bones_influence_vertex>
			MeshVertexData<count_bones_influence_vertex>::MeshVertexData(const aiScene* scene)
				: m_scene(scene)
				, m_meshes(m_scene->mMeshes)
				, SkeletonRoot(nullptr)
			{
				Indices = std::make_shared<std::vector<uint32_t>>(std::vector<uint32_t>());
				Verts = std::make_shared<std::vector<float>>(std::vector<float>());
				T_Verts = std::make_shared<std::vector<float>>(std::vector<float>());
				N_Verts = std::make_shared<std::vector<float>>(std::vector<float>());
				Tangent_Verts = std::make_shared<std::vector<float>>(std::vector<float>());
				Bitanget_Verts = std::make_shared<std::vector<float>>(std::vector<float>());
				BlendWeights = std::make_shared<std::vector<float>>(std::vector<float>());
				BlendIndices = std::make_shared<std::vector<int32_t>>(std::vector<int32_t>());
				GetMeshVertexData();
			}

			template <int32_t count_bones_influence_vertex>
			MeshVertexData<count_bones_influence_vertex>::~MeshVertexData()
			{

			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::GetMeshVertexData()
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

				LoadSkeleton();
				LoadSkin();
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::LoadSkin() {

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
					size_t processedIndices = Indices->size();
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
			void MeshVertexData<count_bones_influence_vertex>::LoadSkeleton() {

				if (bHasAnimation)
				{
					SkeletonRoot = new SkeletonBoneBaseLOADER();
					int32_t boneIdCounter = 0;

					aiNode* rootNode = m_scene->mRootNode->FindNode(GetSkeletonArmatureNodeName(*m_scene->mRootNode).c_str());
					if (rootNode)
					{
						size_t nodesCount = rootNode->mNumChildren;
						for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
						{
							aiNode* childNode = rootNode->mChildren[nodeIndex];
							aiBone* bone = GetBoneByName(std::string(childNode->mName.C_Str()));
							if (bone)
							{
								SkeletonBoneLOADER* skeletonBone = new SkeletonBoneLOADER(SkeletonRoot);
								skeletonBone->SetBoneId(boneIdCounter++);
								skeletonBone->SetBoneInfo(bone);
								FillHierarchyRecursive(childNode, skeletonBone, boneIdCounter);
								SkeletonRoot->AddChildBone(skeletonBone);
							}
						}
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::FillHierarchyRecursive(aiNode* parentNode, SkeletonBoneLOADER*& parentBone, int32_t& boneIdCounter) {

				size_t nodesCount = parentNode->mNumChildren;
				for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
				{
					aiNode* childNode = parentNode->mChildren[nodeIndex];
					SkeletonBoneLOADER* childBone = new SkeletonBoneLOADER(parentBone);
					parentBone->AddChildBone(childBone);
					aiBone* boneInfo = GetBoneByName(std::string(childNode->mName.C_Str()));
					childBone->SetBoneInfo(boneInfo);
					childBone->SetBoneId(boneIdCounter++);
					FillHierarchyRecursive(childNode, childBone, boneIdCounter);
				}
			}

			template <int32_t count_bones_influence_vertex>
			std::string MeshVertexData<count_bones_influence_vertex>::GetSkeletonArmatureNodeName(aiNode& rootNode) {

				std::string name;

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
						name = std::move(std::string(childNode->mName.C_Str()));
					}
				}

				return name;
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::IterateHierarchy(aiNode& parentNode, int32_t& countChildren) {

				size_t nodesCount = parentNode.mNumChildren;
				for (size_t nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++)
				{
					aiNode& childNode = *parentNode.mChildren[nodeIndex];
					++countChildren;
					IterateHierarchy(childNode, countChildren);
				}
			}

			template <int32_t count_bones_influence_vertex>
			aiBone* MeshVertexData<count_bones_influence_vertex>::GetBoneByName(const std::string& name) const {

				aiBone* result = nullptr;

				size_t meshCount = m_scene->mNumMeshes;

				for (size_t currentMeshIndex = size_t(0); (currentMeshIndex < meshCount) && (result == nullptr); currentMeshIndex++)
				{
					aiMesh* mesh = m_meshes[currentMeshIndex];
					size_t bonesCount = mesh->mNumBones;
					for (size_t boneIndex = size_t(0); boneIndex < bonesCount; boneIndex++)
					{
						aiBone* bone = mesh->mBones[boneIndex];
						if (std::string(bone->mName.C_Str()) == name)
						{
							result = bone;
							break;
						}
					}
				}
				return result;
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::CollectIndices(aiMesh* meshBeingCollected, uint32_t lastIndexBeenInterrupted) {

				size_t countOfFaces = meshBeingCollected->mNumFaces;
				for (size_t faceIndex = 0; faceIndex < countOfFaces; faceIndex++)
				{
					aiFace& face = meshBeingCollected->mFaces[faceIndex];
					if (face.mNumIndices == 3) // triangulated face
					{
						Indices->emplace_back(face.mIndices[0] + lastIndexBeenInterrupted);
						Indices->emplace_back(face.mIndices[1] + lastIndexBeenInterrupted);
						Indices->emplace_back(face.mIndices[2] + lastIndexBeenInterrupted);
					}
					else
					{
						throw std::invalid_argument("Face isn't triangulated.");
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::TryToCollectSkinInfo(size_t startIndex, aiMesh* meshBeingCollected) {

            const bool bCollectNormals = meshBeingCollected->HasNormals() & bHasNormals;
            const bool bCollectTexCoords = meshBeingCollected->HasTextureCoords(0) & bHasTextureCoordinates;
            const bool bCollectTangBitang = meshBeingCollected->HasTangentsAndBitangents() & bHasTangentVertices;

				for (size_t attribIndex = 0; attribIndex < meshBeingCollected->mNumVertices; ++attribIndex)
				{
					Verts->emplace_back(meshBeingCollected->mVertices[attribIndex].x);
					Verts->emplace_back(meshBeingCollected->mVertices[attribIndex].y);
					Verts->emplace_back(meshBeingCollected->mVertices[attribIndex].z);

					if (bCollectNormals)
					{
						N_Verts->emplace_back(meshBeingCollected->mNormals[attribIndex].x);
						N_Verts->emplace_back(meshBeingCollected->mNormals[attribIndex].y);
						N_Verts->emplace_back(meshBeingCollected->mNormals[attribIndex].z);
					}
					if (bCollectTexCoords)
					{
						T_Verts->emplace_back(meshBeingCollected->mTextureCoords[0][attribIndex].x);
						T_Verts->emplace_back(meshBeingCollected->mTextureCoords[0][attribIndex].y);
					}
					if (bCollectTangBitang)
					{
						Tangent_Verts->emplace_back(meshBeingCollected->mTangents[attribIndex].x);
						Tangent_Verts->emplace_back(meshBeingCollected->mTangents[attribIndex].y);
						Tangent_Verts->emplace_back(meshBeingCollected->mTangents[attribIndex].z);

						Bitanget_Verts->emplace_back(meshBeingCollected->mBitangents[attribIndex].x);
						Bitanget_Verts->emplace_back(meshBeingCollected->mBitangents[attribIndex].y);
						Bitanget_Verts->emplace_back(meshBeingCollected->mBitangents[attribIndex].z);
					}
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::CollectBlendables(size_t vertexId, std::vector<VertexLOADER>& blendData, aiMesh* meshBeingCollected) {

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
								throw std::invalid_argument("Such bone doesn't exist in skeleton!");
							}
						}
					}
				}

				if (vertex.BoneWeightMap.size() > 0)
               blendData.emplace_back(std::move(vertex));
			}

			template <>
			void MeshVertexData<3>::CollectBlendWeightsAndIndices(VertexLOADER& blendInfoVertex, size_t blendableIndex)
			{
				const size_t currentVertexInfluenceCount = blendInfoVertex.BoneWeightMap.size();

				if (currentVertexInfluenceCount == 1)
				{
					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights->emplace_back(0.0f);
					BlendIndices->emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.

					BlendWeights->emplace_back(0.0f);
					BlendIndices->emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.
				}
				else if (currentVertexInfluenceCount == 2)
				{
					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[1].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[1].first));

					BlendWeights->emplace_back(0.0f);
					BlendIndices->emplace_back(-1);  // this provides assurance that skin matrix will not do anything in shader.
				}
				else if (currentVertexInfluenceCount > 2)
				{
					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[0].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[0].first));

					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[1].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[1].first));

					BlendWeights->emplace_back(blendInfoVertex.BoneWeightMap[2].second);
					BlendIndices->emplace_back(std::get<int32_t>(blendInfoVertex.BoneWeightMap[2].first));
				}
			}

			template <int32_t count_bones_influence_vertex>
			void MeshVertexData<count_bones_influence_vertex>::CleanUp()
			{

			}
		}
	}
}