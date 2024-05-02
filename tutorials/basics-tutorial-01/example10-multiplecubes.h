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
 * @example Example #10 --- Rendering Multiple Cubes
 * @note QUICK NOTE ---- Example #10 will use the same shaders as example #9 because this example is identical just renders multiple cubes
 * @note Going over coordinate systems in computer graphics
 * @note Such as properties part of camera systerms such as the following
 * 
 * @note Where the global picture is the following diagram below
 * 1.) [local space] -> [model matrix] -> [2. World Space ] -> [view matrix]
 * [3. View Space ] -> [projection matrix] -> [4. Clip Space ] -> viewport transform -> [screen space]
 * @note In this example, demonstrates how to draw multiple cubes
 * @note Utilizing glDrawArrays, and utilizing model, view, projection inputs to the vertex shaders.
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

void MultipleCubesExample(GLFWwindow* window){
    printf("Example 10 -- Coordinate Systems\n");
    glEnable(GL_DEPTH_TEST);

    //! @note Feel free to mess around with this and see what the color pallette looks like
    // float vertices[] = {
    //     // positions         // colors
    //     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    //     -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
    //     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    // };

    //! @note NEW --- Adding Texture Coordinates
    // float vertices[] = {
    //     // positions          // colors           // texture coords
    //     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    // };

    // float vertices[] = {
    //     // positions          // texture coords
    //      0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
    //      0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
    //     -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    // };

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


    /**
    
    @note NEW -- TexCoords
    @note In order to map a texture to a triangle (or geometry shape), we need to tell each vertex of traingle
    @note which part of the texture to correspond to, hence having texture coordinates.
    @note In simplification they are, mapping to correspond where to put our textures on a mesh(geometry type of data)
    @note Texture coordinates range from 0 to 1
    @note Getting texture color using texture coords are referred to as sampling.
    @note Where texCoords start at (0, 0) meaning lower-left, (1, 1) meaning upper right, and so on.
    @note Specifying these three coordinates we are specifying how the vertex coorrespond with our textures.
    */

    /**
    @note NEW -- Texture Wrapping
    @note As mentioned texture coordinates range from (0, 0) to (1, 1)
    @note Now we have to look at what happens when specific coordinates are outside this given range.
    @note This is where texture parameters come into context.
    @note OpenGL Options
    @param GL_REPEAT - Default behavior for textures
    @param GL_MIRRORED_REPEAT - Same as GL_REPEAT but mirrors the image
    @param GL_CLAMP_TO_EDGE - Clamps coordinates between 0 and 1. Resulting in higher coordinates being clamped to the edge resulting in stretched pattern
    @param GL_CLAMP_TO_BORDER - Coordinates outside range are given user specific border color
    @note These options have different visual impacts on how the image gets rendered.
    */

    // float texCoords[] = {
    //     0.0f, 0.0f,  // lower-left corner  
    //     1.0f, 0.0f,  // lower-right corner
    //     0.5f, 1.0f   // top-center corner
    // };

    Shader shader("basics/shaders/example09-coordinateSystem/shader.vert", "basics/shaders/example09-coordinateSystem/shader.frag");

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
    shader.Set("inputTexture1", 0);
    shader.Set("inputTexture2", 1);

    
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

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //! @note NEW ---- glDrawElements will automatically assign the texture to our fragment shader
        //! @note Where uniform sampler2D is how we pass our texture from opengl code to our glsl shader
        //! @note When rendering textures (at least in OpenGL), you'll need to bind the texture
        //! @note Before you render that texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, containerTextureID);

        // float time= glfwGetTime();
        // float greenValue = (std::sin(time) / 2.0f) + 0.5f;
        // int vertexInputColor_Location = glad_glGetUniformLocation(programID, "vertexInputColor");
        
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        shader.Bind();

        // glm::mat4 transform = glm::mat4(1.0f);
        // transform = glm::translate(transform, glm::vec3(0.1f, -0.1f, 1.0));
        // transform = glm::rotate(transform, time, glm::vec3(0.0f, 0.0f, 1.0f));
        // shader.Set("transform", transform);

        //! @note V_clip = projection * view * model * V_local;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0);

        // model = glm::rotate(model, time * glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // shader.Set("model", model);

        // How we are passing our transformation matrix to our shaders
        shader.Set("projection", projection);
        shader.SetMat4("view", view);

        glBindVertexArray(vao);

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