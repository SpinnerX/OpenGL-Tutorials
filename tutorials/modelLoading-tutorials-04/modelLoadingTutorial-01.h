#pragma once
#include <string>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

/**
 * @example Model Loading Tutorial #1 - Context for Loading Models (with OpenGL)
 * @param Modeloading
 * @note Showing how to use Assimp to load in assets
 * @note Including in creating meshes using to represents assets that we load.
 * 
 * @param Meshes
 * @note Since assimp can load different models into the application, but once they're stored in assimp data structures
 * @note We'll eventually want to transform this data to a format that OpenGL (or Vulkan/DirectX api you're using) so that we can render these objects
 * @note In this demo, we will be creating three classes Vertex, Texture, and Mesh
 * @note Now, these classes will not be practical but are used for making our example very simple and easy to see where the logic ties into this demo.
 * @note Meshes should be containing where each vertex contains their position, normal, and texture coordinates.
 * @note Mesh also contain indices for index drawing, and material data (in forms of diffuse/specular maps)
 * @note Here we show these basic properties that make our mesh
*/

struct Shader{
    Shader(const std::string& vertex, const std::string& fragment){
        std::unordered_map<GLenum, std::string> sources = ParseShader(vertex, fragment);


        CompileShaders(sources);
    }

    std::unordered_map<GLenum, std::string> ParseShader(const std::string& vertex, const std::string& fragment){
        std::unordered_map<GLenum, std::string> sources;
        std::string vertexShaderCode;
        std::string fragmentShaderCode;

        std::ifstream vertexIns(vertex, std::ios::binary | std::ios::app);
        std::ifstream fragmentIns(fragment, std::ios::binary | std::ios::app);

        if(!vertexIns){
            printf("Could not load vertex shader source!\n");
            // assert(false);
        }

        if(!fragmentIns){
            printf("Could not load fragment shader source!\n");
            // assert(false);
        }

        // vertexIns.open(vertex);
        // fragmentIns.open(fragment);

        std::stringstream vertexSS, fragmentSS;
        vertexSS << vertexIns.rdbuf();
        fragmentSS << fragmentIns.rdbuf();

        vertexIns.close();
        fragmentIns.close();

        vertexShaderCode = vertexSS.str();
        fragmentShaderCode = fragmentSS.str();

        sources[GL_VERTEX_SHADER] = vertexSS.str();
        sources[GL_FRAGMENT_SHADER] = fragmentSS.str();
        return sources;
    }

    void CompileShaders(const std::unordered_map<GLenum, std::string>& sources){
        // uint32_t vertexShaderID, fragmentShaderID;
        // std::array<2, int> shaderIDs;
        std::array<int, 2> shaderIDs;

        // uint32_t shaderID = 0;
        programID = glCreateProgram();
        int success;
        char infoLog[512];
        uint32_t index = 0;
        for(auto[key, value] : sources){
            GLenum type = key;
            const char* shader = value.c_str();

            uint32_t shaderID = glCreateShader(type);
            glShaderSource(shaderID, 1, &shader, nullptr);
            glCompileShader(shaderID);

            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

            //! @note We are checking if this shader has been successful or not.
            //! @note If not then we print out the logging information
            if(!success){
                glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
                std::cout << "Errored our on shader compilation!\n";
                std::cout << "[INFO LOG] ------> " << infoLog << '\n';
            }

            //! @note Storing in our shaders
            shaderIDs[index++] = shaderID;
        }

        //! @note We then attach all of our ID's for the shader
        //! @note Instead of manually attaching both our fragment and vertex we store them in our array and then we attach them once they've compiled successfully
        for(auto id : shaderIDs){
            glAttachShader(programID, id);
        }

        //! @note Then we link them to our program, and then we delete them
        glLinkProgram(programID);

        for(auto id : shaderIDs){
            glDeleteShader(id);
        }
    }

    void Bind() const{
        glUseProgram(programID);
    }

    void Unbind(){
        glUseProgram(0);
    }

    const uint32_t Get(const std::string& name) const{
        return glGetUniformLocation(programID, name.c_str());
    }

    void Set(const std::string& name, bool value) {
        uint32_t location = Get(name);
        glUniform1i(location, value);
    }

    void Set(const std::string& name, int value) {
        uint32_t location = Get(name);
        glUniform1i(location, value);
    }

    void Set(const std::string& name, float value){
        uint32_t location = Get(name);
        glUniform1f(location, value);
    }

    void Set(const std::string& name, glm::vec2 value){
        uint32_t location = Get(name);
        glUniform2f(location, value.x, value.y);
    }

    void Set(const std::string& name, const glm::vec3& values){
        uint32_t location = Get(name);
        glUniform3f(location, values.x, values.y, values.z);
    }

    void Set(const std::string& name, const glm::vec4& values){
        uint32_t location = Get(name);
        glUniform4f(location, values.x, values.y, values.z, values.w);
    }

    void Set(const std::string& name, const glm::mat3& values){
        uint32_t location = Get(name);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(values));
    }

    void Set(const std::string& name, const glm::mat4& values){
        uint32_t location = Get(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
    }

    void SetMat4(const std::string& name, const glm::mat4& value){
        uint32_t location = Get(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

    uint32_t programID;
};

struct Camera{

    void OnUpdate(GLFWwindow* window){
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            cameraPos += cameraSpeed * cameraFront;
        }
        else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            cameraPos -= cameraSpeed * cameraFront;
        }
        else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
    }

    void MouseOnUpdate(double xPosIn, double yPosIn){
        float xpos = static_cast<float>(xPosIn);
        float ypos = static_cast<float>(yPosIn);

        if(firstMouse){
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xOffset = xpos - lastX;
        float yOffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.1f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;

        glm::vec3 direction;
        direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        direction.y = std::sin(glm::radians(pitch));
        direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }

    void MouseOnScrollback(double xOffset, double yOffset){
        zoom = (float)yOffset;

        if(zoom < 1.0f) zoom = 1.0f;
        if(zoom > 45.0f) zoom = 45.0f;
    }
    
    glm::mat4 GetViewMatrix() {
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        return view;
    }

    glm::mat4 GetProjectionMatrix(){
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        return projection;
    }

    glm::mat4 GetDefaultProjection(){
        projection = glm::perspective(glm::radians(zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        return projection;
    }

    //! @note Initializing our view and projection matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    //! @note First we setup our yaw and pitch
    const float cameraSpeed = 0.05f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0f;
    float lastY = 600.0f / 2.0f;
    float zoom = 45.0f;

    bool firstMouse = true;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
};

static uint32_t LoadTexture(const std::string& filepath, GLenum TextureType){
    uint32_t textureID;
    glGenTextures(1, &textureID);

    int w, h, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &channels, 0);

    if(!data){
        printf("Unable to load textures ====> %s\n", filepath.c_str());
        assert(false);
    }
    GLenum format;

    if(channels == 1){
        format = GL_RED;
    }
    else if(channels == 4){
        format = GL_RGBA;
    }
    else if(channels == 3){
        format = GL_RGB;
    }

    glBindTexture(TextureType, textureID);
    glTexImage2D(TextureType, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(TextureType);

    glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

static uint32_t LoadTextureFromFileAsset(const char* file, const std::string& dir, bool isGammaEnabled=false){
    std::string filename(file);
    filename = dir + "/" + filename;

    uint32_t textureID;
    glGenTextures(1, &textureID);
    
    int w, h, channels;
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
    
    if(!data){
        printf("Could not load imaged texture ====> %s\n", filename.c_str());
        assert(false);
        return 0;
    }
    GLenum TextureType = GL_TEXTURE_2D;
    GLenum format;
    if(channels == 1){
        format = GL_RED;
    }
    else if(channels == 4){
        format = GL_RGBA;
    }
    else if(channels == 3){
        format = GL_RGB;
    }

    glBindTexture(TextureType, textureID);
    glTexImage2D(TextureType, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(TextureType);

    glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

// NEW ---- Creating our vertex
struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // tangent
    glm::vec3 BitTangent;
};

// NEW ---- Creating our texture class
struct Texture{
    uint32_t id;
    std::string type;
    std::string path; // Storing path of our texture in comparison to other textures
};

// NEW ---- Creating Mesh Class
class Mesh{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        SetupMesh();
    }

    void Draw(Shader& shader){
        uint32_t diffuseIDs = 1;
        uint32_t specularIDs = 1;

        for(uint32_t i = 0; i < textures.size(); i++){
            glActiveTexture(GL_TEXTURE0 + i); // NEW ------ Activate proper texture before retrieving texture id (the N in diffuse textures of N size)
            std::string number;
            std::string name = textures[i].type;

            if(name == "texture_diffuse"){
                number = std::to_string(diffuseIDs++);
            }
            else if(name == "texture_specular"){
                number = std::to_string(specularIDs++);
            }

            std::string foo("material." + name + number);
            shader.Set(foo.c_str(), (int)i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glActiveTexture(GL_TEXTURE0);

        // Drawing Mesh
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
private:
    uint32_t vao, vbo, ibo;
    void SetupMesh(){
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};
class Model
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.BitTangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = LoadTextureFromFileAsset(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};

static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;

static Camera camera;

void ModelLoadingExample(GLFWwindow* window){
    printf("Lighting Tutorial #2 Part 2 -- Diffuse Lighting Example\n");
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    //! @note Enabling Mouse Inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosIn, double yPosIn){
        camera.MouseOnUpdate(xPosIn, yPosIn);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
        camera.MouseOnScrollback(xOffset, yOffset);
    });

    float skyboxVertices[] = {
        //   Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
        1.0f, -1.0f,  1.0f,//       /|       /|
        1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
        1.0f,  1.0f,  1.0f,//      |/       |/
        1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };

    unsigned int skyboxIndices[] ={
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    std::array<glm::vec3, 4> pointLightPositions;
    pointLightPositions[0] = glm::vec3( 0.7f,  0.2f,  2.0f);
    pointLightPositions[1] = glm::vec3( 2.3f, -3.3f, -4.0f);
    pointLightPositions[2] = glm::vec3(-4.0f,  2.0f, -12.0f);
    pointLightPositions[3] = glm::vec3( 0.0f,  0.0f, -3.0f);

    Shader lightShader("basics/shaders/modelLoading-01/light.vert", "basics/shaders/modelLoading-01/light.frag");
    Shader cubeShader("basics/shaders/modelLoading-01/cube.vert", "basics/shaders/modelLoading-01/cube.frag");

    // uint32_t cubeVao;
    // uint32_t vbo;

    // glGenVertexArrays(1, &cubeVao);
    // glGenBuffers(1, &vbo);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindVertexArray(cubeVao);

    // //! @note Positional location attributes
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // layout (location = 0)
    // glEnableVertexAttribArray(0);

    // // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // layout (location = 1)
    // glEnableVertexAttribArray(1);

    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // layout (location = 2)
    // glEnableVertexAttribArray(2);

    // //! @note Configuring light data
    // uint32_t lightVao;
    // glGenVertexArrays(1, &lightVao);
    // glBindVertexArray(lightVao);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);

    // glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    // glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
    // glm::vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f);

    // glm::vec3 cubeColor = {0.0f, 0.5f, 1.0f};
    // glm::vec3 cubeLightingColor = {1.0f, 1.0f, 1.0f};

    // //! @note Loading our texture
    // uint32_t diffuseMap = LoadTexture("basics/textures/container.png", GL_TEXTURE_2D);

    // //! @note NEW --- Loading our specular container!
    // uint32_t specularMap = LoadTexture("basics/textures/container2_specular.png", GL_TEXTURE_2D);

    // cubeShader.Bind();
    // cubeShader.Set("material.diffuse", 0);
    // cubeShader.Set("material.specular", 1);


    // //! @note NEW ----- Adding Skybox!
    uint32_t skyboxVao, skyboxVbo, skyboxIbo;
    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &skyboxVbo);
    glGenBuffers(1, &skyboxIbo);

    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    std::string parentPath = "basics/figures/skybox-daylight/";
    std::string singleFile = "whiteClouds.jpg";
    std::string faces[] = {
        parentPath + "right.bmp",
        parentPath + "left.bmp",
        parentPath + "top.bmp",
        parentPath + "bottom.bmp",
        parentPath + "front.bmp",
        parentPath + "back.bmp"
    };

    uint32_t cubemapTextureID;
    glGenTextures(1, &cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    for(uint32_t i = 0; i < 6; i++){
        int w, h, channels;
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &channels, 0);

        if(data){
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else{
            printf("Tried to load texture filepath at ===> %s\n", faces[i].c_str());
            printf("Could not load textures!\n");
        }
    }

    Shader skyboxShader("basics/shaders/skybox/skybox.vert", "basics/shaders/skybox/skybox.frag");
    glm::vec3 lightVector = {-2.0f, -1.0f, -0.3f};
    skyboxShader.Bind();
    skyboxShader.Set("skybox", 0);

    //! @note NEW ---- LOADING MODELS
    Shader modelShader("basics/shaders/modelLoading-01/model.vert", "basics/shaders/modelLoading-01/model.frag");
    Model model1("basics/models/backpack.obj");
    printf("Loading Model!\n");

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = static_cast<float>(glfwGetTime());
        deltaTime = time - lastFrame;
        lastFrame = time;
        camera.OnUpdate(window);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        float x = std::sin(time) * 2.0f;
        float z = std::cos(time) * 2.0f;
        float y = 0.0f;




        // Rendering actual lighting shader
        // lightShader.Bind();

        /*
            NEW -------------------------------------------------------------------------------------------------
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        //! @note Directional Lighting
        // lightShader.Set("directLight.direction", {-0.2f, -1.0f, -0.3f});
        // lightShader.Set("directLight.direction", {0.05f, 0.05f, 0.05f});
        // lightShader.Set("directLight.direction", {0.4f, 0.4f, 0.4f});
        // lightShader.Set("directLight.direction", {0.5f, 0.5f, 0.5f});

        // lightShader.Set("pointLights[0].position", pointLightPositions[0]);
        // lightShader.Set("pointLights[0].ambient", {0.05f, 0.05f, 0.05f});
        // lightShader.Set("pointLights[0].diffuse", {0.8f, 0.8f, 0.8f});
        // lightShader.Set("pointLights[0].specular", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("pointLights[0].cpmstant", 1.0f);
        // lightShader.Set("pointLights[0].linear", 0.09f);
        // lightShader.Set("pointLights[0].quadtratic", 0.032f);

        // // point light 2
        // lightShader.Set("pointLights[1].position", pointLightPositions[1]);
        // lightShader.Set("pointLights[1].ambient", {0.05f, 0.05f, 0.05f});
        // lightShader.Set("pointLights[1].diffuse", {0.8f, 0.8f, 0.8f});
        // lightShader.Set("pointLights[1].specular", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("pointLights[1].constant", 1.0f);
        // lightShader.Set("pointLights[1].linear", 0.09f);
        // lightShader.Set("pointLights[1].quadratic", 0.032f);
        // // point light 3
        // lightShader.Set("pointLights[2].position", pointLightPositions[2]);
        // lightShader.Set("pointLights[2].ambient", {0.05f, 0.05f, 0.05f});
        // lightShader.Set("pointLights[2].diffuse", {0.8f, 0.8f, 0.8f});
        // lightShader.Set("pointLights[2].specular", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("pointLights[2].constant", 1.0f);
        // lightShader.Set("pointLights[2].linear", 0.09f);
        // lightShader.Set("pointLights[2].quadratic", 0.032f);
        // // point light 4
        // lightShader.Set("pointLights[3].position", pointLightPositions[3]);
        // lightShader.Set("pointLights[3].ambient", {0.05f, 0.05f, 0.05f});
        // lightShader.Set("pointLights[3].diffuse", {0.8f, 0.8f, 0.8f});
        // lightShader.Set("pointLights[3].specular", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("pointLights[3].constant", 1.0f);
        // lightShader.Set("pointLights[3].linear", 0.09f);
        // lightShader.Set("pointLights[3].quadratic", 0.032f);
        // // spotLight
        // lightShader.Set("spotLight.position", camera.cameraPos);
        // lightShader.Set("spotLight.direction", camera.cameraFront);
        // lightShader.Set("spotLight.ambient", {0.0f, 0.0f, 0.0f});
        // lightShader.Set("spotLight.diffuse", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("spotLight.specular", {1.0f, 1.0f, 1.0f});
        // lightShader.Set("spotLight.constant", 1.0f);
        // lightShader.Set("spotLight.linear", 0.09f);
        // lightShader.Set("spotLight.quadratic", 0.032f);
        // lightShader.Set("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        // lightShader.Set("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   

        // //! @note Defining our actual light source
        // lightShader.Set("light.direction", lightVector);
        // lightShader.Set("light.ambient", {0.2f, 0.2f, 0.2});
        // lightShader.Set("light.diffuse", {0.5f, 0.5f, 0.5f});
        // lightShader.Set("light.specular", {1.0f, 1.0f, 1.0f});

        // //! @note Defining what our materials is
        // lightShader.Set("material.specular", {0.5f, 0.5f, 0.5f});
        // // lightShader.Set("material.shininess", 64.0f);
        // lightShader.Set("material.shininess", 32.0f);

        // // lightShader.Set("light.position", lightPos); // NEW -- Uncomment this to see how shaders works without moving it.
        // // lightShader.Set("light.position", {x, y, z});

        // // view/projection transformation
        modelShader.Bind();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        modelShader.Set("projection", projection);
        modelShader.Set("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.Set("model", model);
        model1.Draw(modelShader);

        // lightShader.Set("projection", projection);
        // lightShader.Set("view", view);
        // glm::mat4 model = glm::mat4(1.0);
        // lightShader.Set("model", model);
        // // lightShader.Set("viewPos", camera.cameraPos);

        // //! @note Bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // // glm::mat4 model = glm::mat4(1.0f);
        // // lightShader.Set("model", model);
        // glBindVertexArray(cubeVao);
        // // cubeShader.Bind();
        // // lightShader.Bind();
        // for(uint32_t i = 0; i < 10; i++){
        //     // glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
        //     float angle = 20.0f * i;
        //     model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //     lightShader.Set("model", model);
        //     // cubeShader.Set("model", model);

        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        //! @note Drawing lamp object
        // cubeShader.Bind();
        // cubeShader.Set("projection", projection);
        // cubeShader.Set("view", view);

        // glBindVertexArray(cubeVao);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(cubeVao);
        // Drawing lamp object
        // Rendering smaller cube as the lamp object
        
        // NEW -- Rendering and setting our lamp color to white here
        // cubeShader.Bind();
        // cubeShader.Set("NewColor", cubeLightingColor);
        // cubeShader.Set("projection", projection);
        // cubeShader.Set("view", view);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, lightPos);
        // // model = glm::translate(model, {x, y, z}); // NEW ---- This is how you move your actual light source object orbiting around the cube
        // model = glm::scale(model, glm::vec3(0.2f)); //! @note Creating smaller cube
        // cubeShader.Set("model", model);

        // cubeShader.Bind();
        // for(uint32_t i = 0; i < pointLightPositions.size(); i++){
        //     cubeShader.Set("NewColor", cubeLightingColor);
        //     cubeShader.Set("projection", projection);
        //     cubeShader.Set("view", view);
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, pointLightPositions[i]);
        //     model = glm::scale(model, glm::vec3(0.2f));
        //     cubeShader.Set("model", model);
        //     glBindVertexArray(lightVao);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }
           
        //! @note Rendering our Model loaded through assimp
        

        // glBindVertexArray(lightVao);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        //! @note NEW ---- Rendering the Skybox here
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Bind();
        view = glm::mat4(glm::mat3(glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp)));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        skyboxShader.Set("view", view);
        skyboxShader.Set("projection", projection);

        glBindVertexArray(skyboxVao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
