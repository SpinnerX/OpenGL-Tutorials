#version 330 core

// Utilized by glAttribPointer is where location mattters
layout (location = 0) in vec3 aPos; // position variable has attribute at position 0
layout (location = 1) in vec2 aTexCoord; // tex coord attribute at position 1


// specifying the color that will get outputted to our fragment shader
out vec2 TexCoord;

uniform mat4 transform;

void main(){
    // gl_Position = vec4(aPos, 1.0);
    gl_Position = transform * vec4(aPos, 1.0);
    // TexCoord = aTexCoord;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}