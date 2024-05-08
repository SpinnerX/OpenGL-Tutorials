#pragma once
#include <string>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @example Lighting Example #01 - Introducing colors in the context of lighting on objects
 * @note This example will go over basic colors in terms of lighting
 * @note Meaning how they work, and how reflections work in terms of objects and lighting
 * @example glm::vec3 coral(1.0f, 0.5f, 0.31f);
 * 
 * Colors
 * @note Colors that we really see in real life are colors that it really is, but reflected color from the object itself.
 * @note Colors aren't absorbed (rejected) by object is color we perceive
 * @note An example is light of sun where it is perceived as white light that is a combination of sum of many different colors
 * @note When shining on a blue toy, will absorb all white colors sub-colors except the blue color.
 * @note Since that toy object does not absorb blue color, therefore will be reflected.
 * @note Reflected light enters our eyes making it look like the toy has a blue color.
 * 
 * Rules of Colors
 * @note These are the rules of color reflection
 * @note When defining a light source (in OpenGL at least), we want to give the light source a color
 * @note Like referencing to the colors mentioned above where we'll give the light source the color white.
 * @note We could then multiply the light sources color with an object's color value, resulting color would be the reflected color of the object (thus being the perceived color)
 * @note Revsiting blue toy example (this time with coral value) and see what are ways to calculate it's perceived color in computer graphics context
 * @note How we can get the resulting color vector by doing a component-wise multiplication between the light and the object color vectors
 * @note QUick Note -- Can get complicated and will involve PBR rendering
  vec3 lightColor(1.0f, 1.0f, 1.0f);
  vec3 toyColor(1.0f, 0.5f, 0.31f);
  vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f);
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

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static Camera camera;

void LightColorsExample(GLFWwindow* window){
    printf("Lighting Tutorial #1 -- Colors Example\n");
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };

    uint32_t indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };

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


    // Shader shader("basics/shaders/example11-camera/shader.vert", "basics/shaders/example11-camera/shader.frag");
    Shader lightShader("basics/shaders/lightTutorial-01/light.vert", "basics/shaders/lightTutorial-01/light.frag");
    Shader cubeShader("basics/shaders/lightTutorial-01/light.vert", "basics/shaders/lightTutorial-01/light.frag");

    uint32_t cubeVao;
    uint32_t vbo;

    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVao);

    //! @note Positional location attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Configuring light data
    uint32_t lightVao;
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Enabling Mouse Inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosIn, double yPosIn){
        camera.MouseOnUpdate(xPosIn, yPosIn);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
        camera.MouseOnScrollback(xOffset, yOffset);
    });

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = static_cast<float>(glfwGetTime());
        deltaTime = time - lastFrame;
        lastFrame = time;
        camera.OnUpdate(window);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        //! @note Rendering
        //! @note Setting view/projectoin/transformation matrix for this cbue
        cubeShader.Bind();
        //! @note Setting our colors for this cube!
        cubeShader.Set("objectColor", {1.0f, 0.5f, 0.31f});
        cubeShader.Set("lightColor", {1.0f, 1.0f, 1.0f});


        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cubeShader.Set("projection", projection);
        cubeShader.Set("view", view);

        // World Transformation
        glm::mat4 model = glm::mat4(1.0f);
        cubeShader.Set("model", model);

        // Rendering Cube lamp
        //! @note Representing our lamp as a cube for rendering
        glBindVertexArray(cubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //! @note Rendering/Drawing Light lamp object
        lightShader.Bind();
        lightShader.Set("objectColor", {1.0f, 1.0f, 0.5f});
        lightShader.Set("lightColor", {1.0f, 1.0f, 1.0f});
        lightShader.Set("projection", projection);
        lightShader.Set("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.Set("model", model);

        glBindVertexArray(cubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // view/projection transformation


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
