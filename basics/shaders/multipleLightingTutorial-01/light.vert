#version 330 core

layout (location = 0) in vec3 aPos; // position variable has attribute at position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


// NEW ---- Now we are showing how to implement model, view, projection matrix
// Formula is = V_clipk = projection * view * model * V_local
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_diffuse2;
// uniform sampler2D texture_diffuse3;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;


void main(){
    FragPos = vec3(model * vec4(aPos,1.0));
    // Normal = aNormal;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_Position = projection * view * vec4(aPos, 1.0);
}