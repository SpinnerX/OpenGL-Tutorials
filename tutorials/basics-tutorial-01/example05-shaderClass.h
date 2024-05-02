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

/**
 * @example Example #5 - Introducing Shaders Class
 * @note In this example instead of manually creating our shader, we are going to create a basic shader class
 * @note This shader class will be used throughout the tutorials moving forwards after this example
 * @note This example will show loading shaders, binding shaders, and setting our uniforms through the shader class
*/

struct Shader{
    Shader(const std::string& vertex, const std::string& fragment){
        // std::unordered_map<GLenum, std::string> sources; //! @note Our actual shader sources
        // sources[GL_VERTEX_SHADER] = vertex;
        // sources[GL_FRAGMENT_SHADER] = fragment;

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

    void Set(const std::string& name, bool value) const{
        uint32_t location = Get(name);
        glUniform1i(location, value);
    }

    void Set(const std::string& name, int value) const {
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

    uint32_t programID;
};

void ShaderClassExample(GLFWwindow* window){
    printf("Example 05 -- Intro to a Shader Class\n");

    // float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left 
    // };
    //! @note Feel free to mess around with this and see what the color pallette looks like
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    uint32_t indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };

    // const char* vertex = R"(
    //     #version 330 core

    //     layout (location = 0) in vec3 aPos;
    //     layout (location = 1) in vec3 aColor; // color variable has attribute position 1

    //     // specifying the color that will get outputted to our fragment shader
    //     out vec3 vertexColor;

    //     void main(){
    //         gl_Position = vec4(aPos, 1.0);
    //         // vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
    //         vertexColor = aColor;
    //     }
    // )";


    // const char* fragment = R"(
    //     #version 330 core
    //     out vec4 FragColor;

    //     // Our input vertex color received from our vertex shader (must be same name and same type)
    //     in vec3 vertexColor;

    //     void main(){
    //         FragColor = vec4(vertexColor, 1.0);
    //     }
    // )";
    Shader shader("basics/shaders/example05-shaders/shader.vert", "basics/shaders/example05-shaders/shader.frag");

    // //! @note Setting up shader process
    // //! @note CompileShader -> AttachShaderProgram -> LinkShaderProgram -> glUseProgram(programID)
    // uint32_t fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShaderID, 1, &fragment, nullptr); //! @note This is how you submit the shader in a format of strings (or const char *)
    // glCompileShader(fragmentShaderID);

    // glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);

    // if(!success){
    //     printf("Fragment Shader Compilation Error!\n");
    // }

    // uint32_t programID = glCreateProgram();
    // glAttachShader(programID, vertexShaderID);
    // glAttachShader(programID, fragmentShaderID);
    // glLinkProgram(programID);

    // glGetProgramiv(programID, GL_LINK_STATUS, &success);

    // if(!success){
    //     glGetProgramInfoLog(programID, 512, nullptr, infoLog);
    //     printf("Shaders progarm could not link properly!!\n");
    //     std::cout << "[INFO LOG] ====> " << infoLog << '\n';
    // }

    // glUseProgram(programID);

    // glDeleteShader(vertexShaderID);
    // glDeleteShader(fragmentShaderID);

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //! @note Once you have compiled and linked your shaders
    //! @note It is required by OpenGL to link in your vertex attributes (so OpenGL knows how to interpet your data)
    //! @note Specifying for attribute at "layout (location = 0)"
    
    //! @note NOTE -- Attribute accessing only one type, at index 0
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Specying attribute at "layout (location = 1)"
    //! @note By setting our attribute data here, this is how we are setting our variable "aColor" from vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time= glfwGetTime();
        float greenValue = (std::sin(time) / 2.0f) + 0.5f;
        // int vertexInputColor_Location = glad_glGetUniformLocation(programID, "vertexInputColor");
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        // glUseProgram(programID);
        shader.Bind(); //! @note Bind means to let us know that we want to write to this shader

        //! @note Now we set our fragment shader variable "vertexInputColor" to values that we are setting
        // glUniform4f(vertexInputColor_Location, 0.0f, 0.0f, 1.0f, 1.0f);
        shader.Set("vertexColor",{0.0f, greenValue, 0.0f, 1.0f});
        // glUniform4f(vertexInputColor_Location, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(vao);
        //! @note glDrawArrays would still work, just modify 3 to the count of how many indices we have. (so if you have 5 indices, put 5)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


}