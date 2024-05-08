#version 330 core
out vec4 FragColor;

// uniform variables that will accept data from opengl code
uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 Normal;

uniform vec3 lightPos; // Set in the OpenGL code
in vec3 FragPos;

uniform vec3 NewColor;

void main(){
    // FragColor = vec4(1.0);
    // FragColor = vec4(1.0);
    FragColor = vec4(NewColor, 1.0);
}