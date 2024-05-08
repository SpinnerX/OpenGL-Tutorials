#include <iostream>
#include <cassert>
// #include "tutorials/basics-tutorial-01/example01-triangle.h"
// #include "tutorials/basics-tutorial-01/example02-index.h"
// #include "tutorials/basics-tutorial-01/example03-shaders.h"
// #include "tutorials/basics-tutorial-01/example04-shaders2.h"
// #include "tutorials/basics-tutorial-01/example05-shaderClass.h"
// #include "tutorials/basics-tutorial-01/example06-textures.h"
// #include "tutorials/basics-tutorial-01/example07-textures2.h"
// #include "tutorials/basics-tutorial-01/example08-transformations.h"
// #include "tutorials/basics-tutorial-01/example09-coordinateSystem.h"
// #include "tutorials/basics-tutorial-01/example10-multiplecubes.h"
// #include "tutorials/basics-tutorial-01/example11-camera.h"
// #include "tutorials/basics-tutorial-01/example12-cameraEuler.h"
// #include "tutorials/basics-tutorial-01/example12-cameraEuler.h"
// #include "tutorials/basics-tutorial-01/example13-cameraClass.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-colors-01.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-lighting-02.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-lighting-02_1.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-lighting-02_2.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-lighting-02_2_diffusionLighting.h"
// #include "tutorials/lighting-tutorial-02/lightingTutorial-lighting-02_3_specularLighting.h"
// #include "tutorials/materials-tutorial-03/materialTutorial-01.h"
// #include "tutorials/lightingMaps-tutorial-04/lightingMapsTutorial-01_1_diffuseMaps.h"
#include "tutorials/lightingMaps-tutorial-04/lightingMapsTutorial-01_2_specularMaps.h"
// #include "tutorials/example-skybox/Skybox.h"

int main(int argc, char** argv){

    if(!glfwInit()){
        std::cout << "glfwInit not working!\n";
        return -2;
    }
    const uint32_t width = 800;
    const uint32_t height = 600;
    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);
    
    GLFWwindow* window = glfwCreateWindow(width, height, "Demo", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    if(!status){
        std::cout << "GLad loader did not work!\n";
        return -2;
    }
    std::cout << "OpenGL Vesion == " << (const char *)glGetString(GL_VERSION) << '\n';

    if(!window){
        std::cout << "Window nullptr!\n";
        return -2;
    }

    // while(!glfwWindowShouldClose(window)){
    //     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);
        
    //     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    // HelloWorldExample(window);
    // IndexBufferExample(window);
    // ShaderAttribExample(window);
    // TextureExample(window);
    // TransformationExample(window);
    // CoordinateSystemExample(window);
    // MultipleCubesExample(window);
    // CameraExample(window);
    // CameraClassExample(window);

    // AmbientLightingExample(window);
    // DiffusionLightingExample(window);
    // SpecularLightingExample(window);
    // MaterialExample(window);
    // DiffuseLightingMapsExample(window);
    SpecularLightingMapsExample(window);


    // ExampleSkybox(window, width, height);

    return 0;
}