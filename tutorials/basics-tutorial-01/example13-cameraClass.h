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
 * @example Example #12 - Camera Euler
 * @note QUICK NOTE -- Similar as Example 11 but we add pitch and yaw to demonstrate both uses
 * @note utilizing euler angles for handling our motion capture with cameras.
 * @note Going over camera position, direction, and view space
 * @note In this example, we show that how to navigate the using our newly camera system
 * @note Though it is quite manual right now but for the sake of learning how this works, it'll be manual for the time being
 * @note Some things to note is we are utilizing yaw, pitch, and delta time
 * @param yaw
 * @param pitch
 * @note We set direction{x, y, z} to trig functions for handling yaw and pitch
 * @note Such as direction.x = cos(rad(yaw)) * cos(rad(pitch))
 * @note direction.y = sin(rad(pitch))
 * @note direction.z = sin(rad(yaw)) * cos(rad(pitch));
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
        fov = (float)yOffset;

        if(fov < 1.0f) fov = 1.0f;
        if(fov > 45.0f) fov = 45.0f;
    }
    
    glm::mat4 GetViewMatrix() {
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        return view;
    }

    glm::mat4 GetProjectionMatrix(){
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        return projection;
    }

    //! @note Initializing our view and projection matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    //! @note First we setup our yaw and pitch
    const float cameraSpeed = 0.05f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0f;
    float lastY = 600.0f / 2.0f;
    float fov = 45.0f;

    bool firstMouse = true;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
};

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static Camera camera;
void CameraClassExample(GLFWwindow* window){
    printf("Example 12 -- Coordinate Systems\n");
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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


    Shader shader("basics/shaders/example11-camera/shader.vert", "basics/shaders/example11-camera/shader.frag");

    //! @note The process is create and bind your vertex arrays before creating and binding your vertex/index buffers. (in that order)
    uint32_t vao; // vertex array id
    uint32_t vbo; // vertex buffer id
    uint32_t ibo; // index buffer id
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    //! @note Need to bind the vertex array first before binding vertex buffer/index buffer
    glBindVertexArray(vao);
    
    //! @note These should be binded after binding vertex arrayts
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //! @note Doing the same thing as the two lines above but specifically for index buffers
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //! @note Reference to example05 or previous examples on vertex attributes
    //! @note NEW ---- We adjust the 6 * sizeof(float) to 8 * sizeof(float)
    //! @note This is because we are changing it to 8 beacuse we are counting how many bytes our types are total.
    //! @note In this example we know we have two vec3's and one vec2, where each value is a single byte
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Specying attribute at "layout (location = 1)"
    //! @note By setting our attribute data here, this is how we are setting our variable "aColor" from vertex shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //! @note Now we are setting our texture attribute from our vertex shader through here.
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    //! @note First we load our image
    int imageWidth, imageHeight, pixelChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load("basics/textures/container.jpg", &imageWidth, &imageHeight, &pixelChannels, 0);


    // Creating our textures for use here!!
    //! @note Once we load our image
    //! @note We then use that data to generate our texture
    //! @note How we generate our texture is similar to creating a vertex buffer format
    //! @note Only thing is we have to specifiy a GLenum like GL_TEXTURE_2D or GL_TEXTURE_3D
    uint32_t brickTextureID;
    glGenTextures(1, &brickTextureID);
    glActiveTexture(GL_TEXTURE0); //! @note Activates the texture unit first before you bind that texture
    glBindTexture(GL_TEXTURE_2D, brickTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!data){
        printf("Coult not load file for texture!\n");
        assert(false);
    }
    else{
        //! @note Then we specify our 2D texture
        //! @note Including specifying our pixel channels with how our image dimensions are correspond with the data passed.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        //! @note glGenerateMipMap automatically generates all required mipmaps for our current bounded texture
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    //! @note Once you create the texture, you free that data since we do not need it anymore.
    stbi_image_free(data);


    // NEW --- Creating our second texture to mix with the first texture
    // Loading our second image
    int imageWidth2, imageHeight2, pixelChannels2;
    unsigned char* data2 = stbi_load("basics/textures/awesomeface.png", &imageWidth2, &imageHeight2, &pixelChannels2, 0);
    uint32_t containerTextureID;
    glGenTextures(1, &containerTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, containerTextureID);

    //! @note Setting our texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //! @note Setting our texture filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!data2){
        printf("Could not load file for data2!");
        assert(false);
    }
    else{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth2, imageHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader.Bind(); //! @note Bind means to let us know that we want to write to this shader

    //! @note In vertex shader writing to inputTexture1 and inputTexture2
    shader.Set("inputTexture1", 0);
    shader.Set("inputTexture2", 1);

    //! @note Enabling Mouse Inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosIn, double yPosIn){
        camera.MouseOnUpdate(xPosIn, yPosIn);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
        camera.MouseOnScrollback(xOffset, yOffset);
    });


    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = static_cast<float>(glfwGetTime());
        deltaTime = time - lastFrame;
        lastFrame = time;
        camera.OnUpdate(window);


        // float greenValue = (std::sin(time) / 2.0f) + 0.5f;
        // int vertexInputColor_Location = glad_glGetUniformLocation(programID, "vertexInputColor");
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        shader.Bind();

        //! @note V_clip = projection * view * model * V_local;
        
        //! @note We first set our view and projection matrix
        // glm::mat4 view = glm::mat4(1.0f);
        // view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);

        // glm::mat4 projection = glm::mat4(1.0);
        // projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        shader.SetMat4("view", camera.GetViewMatrix());
        shader.Set("projection", camera.GetProjectionMatrix());

        glBindTexture(GL_TEXTURE_2D, brickTextureID);

        glBindTexture(GL_TEXTURE_2D, containerTextureID);

        glBindVertexArray(vao);

        //! @note Then we set our model matrix which is where we are going to set our cubes instances in our world space
        for(uint32_t i = 0; i < 10; i++){
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.Set("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
