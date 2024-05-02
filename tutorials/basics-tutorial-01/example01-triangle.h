#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * @example Example #1
 * @param VertexShader
 * @param FragmentShader
 * @note The process in creating a shader goes
 * @note Every time you create a shader, in OpenGL you'll be doind this process
 * @note [shader source : str] -> glCreateShader(shader_t) -> glCompileShader(id) -> glUseProgram(shaderID) -> createProgram(p_id) -> attach(shaderID) -> glLinkProgram(p_id) -> then delete your shaders
 * @note Where you will delete your shaders since you already have linked your shaders to this shader program!
 * 
 * 
 * @param vao
 * @param vbo
 * @note One thing to note is that the order in which you bind these matter
 * 
 * @note First thing you do is create using glGen* your vertex array, then vertex buffer
 * @note Once you create them, you bind your vertex array before binding your vertex buffer
 * @note Now what this means is that you are going to be configuring your vertex attributes
 * @note Hence why we use glGAttribPointer (which you can only configure this once you've binded vertex buffer beforehand)
 * 
 * @note Once you have configured vertex attribtues then you unbind your vertex buffer before unbinding your vertex array
 * @note When you unbind it this way, that means that your vertex array wont accidentally modify your vertex array
 * @note Though it can rarely happen because in order to modify your vertex array you need to call glBindVertexArray for that to happen.
 * @note Normally vertex arrays are not normally unbinded (nor vertex buffers) when not directly necessary
 * @note In this example we do it just in case we accidentally modify them
 * 
 * 
 * 
*/

//! @note The process when using vertex buffers, with vertex arrays
//! @note We first create our vertex data
//! @note First we setup vertex data inside vertex buffer object
//! @note Then we set that data to our buffer that must have an ID. (hence uint32_t of vbo)
//! @note Then after we set the data with glBufferData we bind that buffer before use
//! @note Binding buffer is equivalent to submitting to renderer command queue (refernecing to vulkan)

void HelloWorldExample(GLFWwindow* window){
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
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
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


}