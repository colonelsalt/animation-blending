#include "Model.h"

#include "TextureHelper.h"

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
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error loading model with Assimp: " << importer.GetErrorString() << std::endl;
		exit(1);
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

		std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, TextureType::Diffuse);
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

		std::vector<Texture> specularTextures = LoadMaterialTextures(material, TextureType::Specular);
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, TextureType type)
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
			// Assuming that textures are all in same directory as model
			texture.Id = TextureHelper::LoadTexture(textureFileName.c_str(), m_DirectoryPath);
			texture.Type = type;
			texture.FileName = textureFileName;
			
			textures.push_back(texture);

			// Cache texture data in case it's used by another mesh later
			m_TexturesLoaded[textureFileName] = texture;
		}

	}

	return textures;
}
