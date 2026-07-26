#include "engine/render/ModelLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <SDL3/SDL_log.h>

Mesh ModelLoader::Load(const std::string& path) {
    Assimp::Importer importer;
    
    // Import with triangulation + generate normals if missing
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs
    );
    
    if (!scene || !scene->mRootNode || !scene->mNumMeshes) {
        SDL_Log("Failed to load model: %s", importer.GetErrorString());
        return Mesh::createCube(); // fallback to cubes
    }
    
    // Take the first mesh for now (expand later)
    aiMesh* aiMesh = scene->mMeshes[0];
    Mesh mesh;
    
    // Copy vertices
    mesh.vertices.reserve(aiMesh->mNumVertices);
    for (unsigned int i = 0; i < aiMesh->mNumVertices; ++i) {
        Vertex vert;
        vert.position.x = aiMesh->mVertices[i].x;
        vert.position.y = aiMesh->mVertices[i].y;
        vert.position.z = aiMesh->mVertices[i].z;
        
        if (aiMesh->HasNormals()) {
            vert.normal.x = aiMesh->mNormals[i].x;
            vert.normal.y = aiMesh->mNormals[i].y;
            vert.normal.z = aiMesh->mNormals[i].z;
        }
        
        if (aiMesh->HasTextureCoords(0)) {
            vert.uv.x = aiMesh->mTextureCoords[0][i].x;
            vert.uv.y = aiMesh->mTextureCoords[0][i].y;
        } else {
            vert.uv = {0.0f, 0.0f};
        }
        
        mesh.vertices.push_back(vert);
    }
    
    // Copy indices
    for (unsigned int i = 0; i < aiMesh->mNumFaces; ++i) {
        aiFace& face = aiMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }
    
    SDL_Log("Loaded model: %s (%zu vertices, %zu indices)", 
            path.c_str(), mesh.vertices.size(), mesh.indices.size());
    
    return mesh;
}