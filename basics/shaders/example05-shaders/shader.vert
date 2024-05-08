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