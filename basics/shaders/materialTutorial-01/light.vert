#version 330 core

layout (location = 0) in vec3 aPos; // position variable has attribute at position 0
layout (location = 1) in vec3 aNormal;


// NEW ---- Now we are showing how to implement model, view, projection matrix
// Formula is = V_clipk = projection * view * model * V_local
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

out vec3 FragPos;
out vec3 Normal;


void main(){
    FragPos = vec3(model * vec4(aPos,1.0));
    Normal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}