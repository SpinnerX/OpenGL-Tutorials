#version 330 core
out vec4 FragColor;

// uniform variables that will accept data from opengl code
uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 Normal;

uniform vec3 lightPos; // Set in the OpenGL code
in vec3 FragPos;
uniform vec3 viewPos; // NEW -- viewpos is simply taking the position vector of camera

void main(){
    // FragColor = vec4(1.0);

    // Setting the intensity of the ambient lighting
    // How you dad the ambient lighting is by doing the following
    // -- Using light colors and multiply with a small constant ambient factor
    // -- Multiplyinh that with object's initial color
    // -- Using fragment color in cube object-shaders (in this example this shader
    // What that looks like is the following code below.
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = normalize(Normal);

    vec3 lightDirection = normalize(lightPos - FragPos);

    // NEW ----- specular lighting
    // NEW ----- Reflect function expects first vector which are points away from light source towards fragment positions.
    // NEW ----- Light Direction cector is currently pointing the other way around; from fragment toward light source
    // NEW ----- Making sure we get correct reflection vector is by reversing it's direction (hence -lightDirection) by negating light direction vector first
    // NEW ----- Second argument expects the normal vector so then you supply it with the normalized viewDirection.
    // NEW ----- Doing the following here
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);

    // NEW ----- Our actual specular lighting is done through here!
    vec3 specular = specularStrength * spec * lightColor;

    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}