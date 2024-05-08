#version 330 core
out vec4 FragColor;

// Our input vertex color received from our vertex shader (must be same name and same type)
in vec3 vertexColor;

out vec3 vertexInputColor;
uniform vec3 openglInputColor;

void main(){
    // FragColor = vec4(vertexColor, 1.0);
    // FragColor = vec4(vertexInputColor, 1.0);
    FragColor = vec4(openglInputColor, 1.0);
}