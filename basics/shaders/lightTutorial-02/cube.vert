#version 330 core

layout (location = 0) in vec3 aPos; // position variable has attribute at position 0



// NEW ---- Now we are showing how to implement model, view, projection matrix
// Formula is = V_clipk = projection * view * model * V_local
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}