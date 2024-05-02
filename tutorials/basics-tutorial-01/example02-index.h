#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * @example Example #2
 * @note In this example will go over how shaders work in OpenGL more thoroughly
 * @note The first example HelloWorld just goes over how vertex buffers correspond to a very basic shader
 * @note This second example is building on top of the first example in helloworld01.h
 * @note Though this example will go over how index buffers will work in OpenGL
 * @note And how does OpenGL handles vertex and index buffers
*/

/*
@note This is visually how the index buffers are referenced to our vertices data
@note This applies the same way as the indices shown below
1         2
 +-------+
 |     / |
 |   /   |
 | /     |
 +-------+
0         3

uint32_t indices[] = {
    0, 1, 2,
    0, 2, 3
}

*/

void IndexBufferExample(GLFWwindow* window){
    printf("Example 02 -- Index/Element Buffer Objects\n");
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    uint32_t indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };

    //! @note Setting up Vertex Shader
    //! @note The process of creating of the shader is as follows.

    //! @note When doing shaders
    //! @note You start by creating a shader and setting what kind of shader will you want to compile
    //! @note Because the process is [shader source] -> createShader -> compileShader -> linkShader
    //! @note We use vertexShaderID or fragmentShaderID to indicate what shaders we want to use.
    const char* vertex = R"(
        #version 330 core

        layout (location = 0) in vec3 aPos;

        void main(){
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    //! @note To create a shader you need these following three lines to create the shader
    //! @note In this order, one thing to note is glAttachShader is where you'd attach both your vertex & fragment shaders
    //! @note glCreateShader() -> glShaderSource() -> glCompileShader() -> glCreateProgram -> glAttachShader(shaderID) -> glLinkProgram(p_id);
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

        void main(){
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";

    //! @note Setting up shader process
    //! @note CompileShader -> AttachShaderProgram -> LinkShaderProgram -> glUseProgram(programID)
    uint32_t fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragment, nullptr); //! @note This is how you submit the shader in a format of strings (or const char *)
    glCompileShader(fragmentShaderID);

    uint32_t programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if(!success){
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
    }

    glUseProgram(programID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    //! @note Once you specify your attributes for the given data, you now create a vertex array that processes these information
    uint32_t vao;
    uint32_t vbo;
    uint32_t ibo;
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
    //! @note It is required by OpenGL to link in your vertex attributes
    //! @note Essentially this is how you tell OpenGL how to interpret our data
    //! @note These are parameters that glkVertexAttribPointer takes
    //! @param index - uint32_t is what we specify where we want to configure. Remembering that in our "vertex" shader we specified layout (location = 0). This sets our location of the vertex attribute to 0, since we want to pass this data to this vertex attribute. Therefore specifying this first param to zero (0).
    //! @param size - uint32_t is for specifying the size of our vertex attribute. In this case we specified the vertex attribute is of vec3. So composed of three floats. You compute the size of the type itself. Which is why we specify 3 * sizeof(float) because vec3 = {float, float, float} and has to be represented in bytes.
    //! @param type - type (such as GL_FLOAT, etc)
    //! @param normalized - boolean for setting if we want to normalized our data (-1 meaning for signed data)
    //! @param stride - tells us space between vertex attributes. Since next set positinoal data are exactly 3x size of float, then that is the value specified.
    //! @param pointer - This requires some weird cast. Though represents our offset of where data begins in our buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        glUseProgram(programID);
        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


}