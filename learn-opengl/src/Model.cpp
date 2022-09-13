#include "Model.h"

#include "Core.h"

#include "TextureHelper.h"
#include "AssimpHelper.h"

static aiTextureType ToAssimpTextureType(TextureType type)
{
	switch (type)
	{
		case TextureType::Diffuse:
			return aiTextureType_DIFFUSE;
		case TextureType::Specular:
			return aiTextureType_SPECULAR;
		default:
			std::cout << "Something went very wrong; invalid texture type!" << std::endl;
	}
}

Model::Model(const char* path)
{
	LoadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (uint32_t i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].Draw(shader);
}

void Model::LoadModel(const std::string& path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error loading model with Assimp: " << importer.GetErrorString() << std::endl;
		__debugbreak();
	}
	m_DirectoryPath = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// Process all meshes in node
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	// Process all node's children
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	// Process vertex data
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		vertex.ResetJointData();

		if (mesh->mTextureCoords[0]) // first set of coords is nullptr if no texture coords are present in this vertex
		{
			// Each vertex may have up to 8 tex coords - we'll only use the first set
			float texX = mesh->mTextureCoords[0][i].x;
			float texY = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = { texX, texY };
		}
		else
			vertex.TexCoords = { 0.0f, 0.0f };

		vertices.push_back(vertex);
	}

	// Process face indices
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process mesh materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, TextureType::Diffuse, scene);
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

		std::vector<Texture> specularTextures = LoadMaterialTextures(material, TextureType::Specular, scene);
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
	}

	ExtractJoints(vertices, mesh, scene);

	//std::cout << "Created mesh: " << mesh->mName.C_Str() << std::endl;
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, TextureType type, const aiScene* scene)
{
	std::vector<Texture> textures;
	
	aiTextureType aiType = ToAssimpTextureType(type);
	for (uint32_t i = 0; i < material->GetTextureCount(aiType); i++)
	{
		aiString fileName;
		material->GetTexture(aiType, i, &fileName);
		std::string textureFileName = std::string(fileName.C_Str());

		if (m_TexturesLoaded.find(textureFileName) != m_TexturesLoaded.end())
		{
			// No need to load texture again if we've loaded it before
			textures.push_back(m_TexturesLoaded[textureFileName]);
		}
		else
		{
			Texture texture;
			if (false)
			{
				// Assuming that textures are all in same directory as model
				texture.Id = TextureHelper::LoadTexture(textureFileName.c_str(), m_DirectoryPath);
				texture.Type = type;
				texture.FileName = textureFileName;
			}
			else
			{
				// Assuming textures are embedded in model file
				const aiTexture* aiTexture = scene->GetEmbeddedTexture(textureFileName.c_str());
				texture.Id = TextureHelper::LoadTextureEmbedded(aiTexture);
				texture.Type = type;
				texture.FileName = textureFileName;
			}
			
			textures.push_back(texture);

			// Cache texture data in case it's used by another mesh later
			m_TexturesLoaded[textureFileName] = texture;
		}

	}

	return textures;
}

void Model::ExtractJoints(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	// Go through all joints in mesh
	for (uint32_t i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		
		std::string jointName = bone->mName.C_Str();
		int jointId = AppendJoint(jointName, AssimpHelper::AssimpToGlmMatrix(bone->mOffsetMatrix));

		S_ASSERT(jointId != -1);
		
		uint32_t numVerticesInBone = bone->mNumWeights;
		aiVertexWeight* weights = bone->mWeights;

		// Go through all vertices whose position is influenced by this joint
		for (uint32_t j = 0; j < numVerticesInBone; j++)
		{
			uint32_t vertexId = weights[j].mVertexId;
			float weight = weights[j].mWeight;

			S_ASSERT(vertexId < vertices.size());
			vertices[vertexId].SetJointData(jointId, weight);
		}

	}
}

int Model::AppendJoint(const std::string& name, const glm::mat4& inverseBindPose)
{
	// Joint may have been seen before in a different mesh in this model
	if (m_JointIndex.find(name) == m_JointIndex.end())
	{
		Joint joint;
		joint.Id = m_NumJointsLoaded;
		joint.InverseBindPose = inverseBindPose;

		m_JointIndex[name] = joint;
		m_NumJointsLoaded++;
	}
	return m_JointIndex[name].Id;
}
