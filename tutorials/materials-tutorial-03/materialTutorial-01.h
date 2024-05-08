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
 * @example Material Tutorial #1 - Materials
 * 
 * @param Materials
 * @note For some context each object has different reaction to light
 * @note Such as Steel objects are often shinier than clay vase, whereas wooden container doesn't react in comparison to steel.
 * @note Some objects reflect light without much scattering resulting in small specular highlights and others scatter a lot giving highlight a large radius.
 * @note In the sense to simulate several types of objects (at least in OPENGL) you would initially define a materials structure that define properties for each surface.
 * @note In reference to our previous demo (specifically specular lighting example tutorial #2 part 3)
 * @note we define visual output of the object combining ambient and specular intensity component.
 * @note When describing a surface we can define a material color for each 3 lighting component; ambient, diffuse, and specular lighting. 
 * @note By specifying a color for each components, having fine-grained control over the color output of the surface.
 * @note Regards to adding shininess component to those 3 colors, you'll have all maaterial properties needed (look in the fragment shader in basics/shaders/materialTutorial-01/light.frag)
 * 
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

static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;

static Camera camera;

void MaterialExample(GLFWwindow* window){
    printf("Lighting Tutorial #2 Part 2 -- Diffuse Lighting Example\n");
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    Shader lightShader("basics/shaders/materialTutorial-01/light.vert", "basics/shaders/materialTutorial-01/light.frag");
    Shader cubeShader("basics/shaders/materialTutorial-01/cube.vert", "basics/shaders/materialTutorial-01/cube.frag");

    uint32_t cubeVao;
    uint32_t vbo;

    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVao);

    //! @note Positional location attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //! @note Configuring light data
    uint32_t lightVao;
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Enabling Mouse Inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosIn, double yPosIn){
        camera.MouseOnUpdate(xPosIn, yPosIn);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
        camera.MouseOnScrollback(xOffset, yOffset);
    });

    // glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
    glm::vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f);

    glm::vec3 cubeColor = {0.0f, 0.5f, 1.0f};
    glm::vec3 cubeLightingColor = {1.0f, 1.0f, 1.0f};

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

        glm::vec3 lightColorChanging;
        lightColorChanging.x = std::sin(time * 2.0f);
        lightColorChanging.y = std::sin(time * 0.7f);
        lightColorChanging.z = std::sin(time * 1.3f);

        glm::vec3 diffuse = lightColorChanging * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuse * glm::vec3(0.2f);


        // Rendering actual lighting shader
        lightShader.Bind();

        //! @note Defining our actual light source
        // lightShader.Set("light.ambient", {0.2f, 0.2f, 0.2f});
        // lightShader.Set("light.diffuse", {0.5f, 0.5f, 0.5f});
        lightShader.Set("light.ambient", ambientColor);
        lightShader.Set("light.diffuse", diffuse);
        lightShader.Set("light.specular", {1.0f, 1.0f, 1.0f});

        //! @note Defining what our materials is
        lightShader.Set("material.ambient", {1.0f, 0.5f, 0.31f});
        lightShader.Set("material.diffuse", {1.0f, 0.5f, 0.31f});
        lightShader.Set("material.specular", {0.5f, 0.5f, 0.5f});
        lightShader.Set("material.shininess", 32.0f);

        // lightShader.Set("objectColor", {1.0f, 0.5f, 0.31f});
        lightShader.Set("objectColor", cubeColor); // This is how you set the actual cube color that is reflecting off of the light source
        lightShader.Set("lightColor", {1.0f, 1.0f, 1.0f});
        // // lightShader.Set("lightPos", lightPos); // NEW -- Uncomment this to see how shaders works without moving it.
        lightShader.Set("lightPos", {x, y, z}); // NEW --- Uncomment this to see the shaders handle lighting (to move it)

        // view/projection transformation
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        lightShader.Set("projection", projection);
        lightShader.Set("view", view);
        lightShader.Set("viewPos", camera.cameraPos);

        glm::mat4 model = glm::mat4(1.0f);
        lightShader.Set("model", model);

        glBindVertexArray(cubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Drawing lamp object
        // Rendering smaller cube as the lamp object
        cubeShader.Bind();

        // NEW -- Setting our lamp color to white
        cubeShader.Set("NewColor", cubeLightingColor);
        cubeShader.Set("projection", projection);
        cubeShader.Set("view", view);
        model = glm::mat4(1.0f);
        // model = glm::translate(model, lightPos);
        model = glm::translate(model, {x, y, z}); // NEW ---- This is how you move your light source orbiting around the cube
        model = glm::scale(model, glm::vec3(0.2f)); //! @note Creating smaller cube
        cubeShader.Set("model", model);

        glBindVertexArray(lightVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
