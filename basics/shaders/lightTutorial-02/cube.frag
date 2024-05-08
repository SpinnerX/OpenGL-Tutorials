#version 330 core
out vec4 FragColor;

// uniform variables that will accept data from opengl code
uniform vec3 objectColor;
uniform vec3 lightColor;

void main(){
    // FragColor = vec4(1.0);

    // Setting the intensity of the ambient lighting
    // How you dad the ambient lighting is by doing the following
    // -- Using light colors and multiply with a small constant ambient factor
    // -- Multiplyinh that with object's initial color
    // -- Using fragment color in cube object-shaders (in this example this shader
    // What that looks like is the following code below.
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(lightColor * objectColor, 1.0);
    // FragColor = vec4(1.0);
}