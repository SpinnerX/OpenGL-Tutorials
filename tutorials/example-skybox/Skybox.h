#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
            assert(false);
        }

        if(!fragmentIns){
            printf("Could not load fragment shader source!\n");
            assert(false);
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
    const float cameraSpeed = 0.005f;
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

static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;

static Camera camera;

void ExampleSkybox(GLFWwindow* window, uint32_t width, uint32_t height){
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
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

    unsigned int indices[] ={
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

    uint32_t skyboxVao, vbo, ibo;

    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string parentPath = "basics/figures/skybox-daylight/";
    std::string singleFile = "whiteClouds.jpg";

    // std::string faces[] = {
    //     parentPath + "right.jpg",
    //     parentPath + "left.jpg",
    //     parentPath + "top.jpg",
    //     parentPath + "bottom.jpg",
    //     parentPath + "front.jpg",
    //     parentPath + "back.jpg"
    // };
    // std::string faces[] = {
    //     parentPath + "whiteClouds.jpg",
    //     parentPath + "whiteClouds.jpg",
    //     parentPath + "whiteClouds.jpg",
    //     parentPath + "whiteClouds.jpg",
    //     parentPath + "whiteClouds.jpg",
    //     parentPath + "whiteClouds.jpg"
    // };
    // std::string faces[] = {
    //     parentPath + "earthIllumination.jpg",
    //     parentPath + "earthIllumination.jpg",
    //     parentPath + "earthIllumination.jpg",
    //     parentPath + "earthIllumination.jpg",
    //     parentPath + "earthIllumination.jpg",
    //     parentPath + "earthIllumination.jpg"
    // };

    std::string faces[] = {
        parentPath + "right.bmp",
        parentPath + "left.bmp",
        parentPath + "top.bmp",
        parentPath + "bottom.bmp",
        parentPath + "front.bmp",
        parentPath + "back.bmp"
    };


    uint32_t cubeTextureID;
    glGenTextures(1, &cubeTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
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
    glm::vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec3 lightPos = {0.5f, 0.5f, 0.5f};

    Shader shader("basics/shaders/skybox/skybox.vert", "basics/shaders/skybox/skybox.frag");
    Shader cubeShader("basics/shaders/lightTutorial-02/cube.vert", "basics/shaders/lightTutorial-02/cube.frag");

    shader.Bind();
    shader.Set("skybox", 0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosIn, double yPosIn){
        camera.MouseOnUpdate(xPosIn, yPosIn);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
        camera.MouseOnScrollback(xOffset, yOffset);
    });

    while(!glfwWindowShouldClose(window)){
        // Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);

        camera.OnUpdate(window);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        

        shader.Bind();
        // glm::mat4 view = glm::mat4(1.0f);
        // glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = camera.GetProjectionMatrix();

        view = glm::mat4(glm::mat3(glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp)));
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        shader.Set("view", view);
        shader.Set("projection", projection);

        glBindVertexArray(skyboxVao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        // Enables Cull Facing
        // glEnable(GL_CULL_FACE);
        // // Keeps front faces
        // glCullFace(GL_FRONT);
        // // Uses counter clock-wise standard
        // glFrontFace(GL_CCW);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // shader.Set("lightColor", {light})


};
