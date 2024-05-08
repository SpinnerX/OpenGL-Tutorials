#version 330 core

// Utilized by glAttribPointer is where location mattters
layout (location = 0) in vec3 aPos; // position variable has attribute at position 0
layout (location = 1) in vec3 aColor; // color variable has attribute position 1
layout (location = 2) in vec2 aTexCoord; // tex coord attribute at position 2


// specifying the color that will get outputted to our fragment shader
out vec3 vertexColor;
out vec2 TexCoord;

void main(){
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
    TexCoord = aTexCoord;
}