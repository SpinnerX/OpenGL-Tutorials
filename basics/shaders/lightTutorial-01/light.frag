#version 330 core
out vec4 FragColor;

// uniform variables that will accept data from opengl code
uniform vec3 objectColor;
uniform vec3 lightColor;

void main(){
    // FragColor = vec4(1.0);
    FragColor = vec4(lightColor * objectColor, 1.0);
    // FragColor = vec4(1.0);
}