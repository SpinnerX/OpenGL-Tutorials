#pragma once
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * @example Example #4 - Shaders part 2
 * @note Second part in learning shaders
 * @note In this example you modify the shaders by adding colors to the vertices themselves
 * @note Rather then specifying them through uniforms
*/

void ShaderAttribExample(GLFWwindow* window){
    printf("Example 04 -- Shaders Part 2\n");

    // float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left 
    // };
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

    const char* vertex = R"(
        #version 330 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor; // color variable has attribute position 1

        // specifying the color that will get outputted to our fragment shader
        out vec3 vertexColor;

        void main(){
            gl_Position = vec4(aPos, 1.0);
            // vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
            vertexColor = aColor;
        }
    )";

    uint32_t vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertex, nullptr);
    glCompileShader(vertexShaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);

    if(!success){
        std::cout << "Vertex Shader Compilation Error!\n";
    }

    const char* fragment = R"(
        #version 330 core
        out vec4 FragColor;

        // Our input vertex color received from our vertex shader (must be same name and same type)
        in vec3 vertexColor;

        void main(){
            FragColor = vec4(vertexColor, 1.0);
        }
    )";

    //! @note Setting up shader process
    //! @note CompileShader -> AttachShaderProgram -> LinkShaderProgram -> glUseProgram(programID)
    uint32_t fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragment, nullptr); //! @note This is how you submit the shader in a format of strings (or const char *)
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);

    if(!success){
        printf("Fragment Shader Compilation Error!\n");
    }

    uint32_t programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if(!success){
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        printf("Shaders progarm could not link properly!!\n");
        std::cout << "[INFO LOG] ====> " << infoLog << '\n';
    }

    glUseProgram(programID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //! @note Specying attribute at "layout (location = 1)"
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // float time= glfwGetTime();
        // float greenValue = (std::sin(time) / 2.0f) + 0.5f;
        // int vertexInputColor_Location = glad_glGetUniformLocation(programID, "vertexInputColor");
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        glUseProgram(programID);

        //! @note Now we set our fragment shader variable "vertexInputColor" to values that we are setting
        // glUniform4f(vertexInputColor_Location, 0.0f, 0.0f, 1.0f, 1.0f);
        // glUniform4f(vertexInputColor_Location, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(vao);
        //! @note glDrawArrays would still work, just modify 3 to the count of how many indices we have. (so if you have 5 indices, put 5)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


}