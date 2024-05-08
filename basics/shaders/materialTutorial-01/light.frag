#version 330 core
out vec4 FragColor;


// NEW --- Specifying our materials properties
// NEW --- Specifying lighting (3 ways) and shininess of this material
struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// NEW ---- Specfying our Light Component
struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// NEW --- Our material set through opengl code
uniform Material material;

// NEW --- Our representation of a light source
uniform Light light;

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
    
    // ambient
    // vec3 ambient = ambientStrength * lightColor;
    // vec3 ambient = ambientStrength * material.ambient;
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffused Lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDirection), 0.0);
    // NEW ---- Utilized our defined material property
    // vec3 diffuse = diff * lightColor;
    // vec3 diffuse = lightColor * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular lighting
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    // NEW ---- Utilized our defined material property
    // float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    // NEW ---- Utilized our defined material property
    // vec3 specular = specularStrength * spec * lightColor; // Our actual specular lighting is done through here!
    // vec3 specular = lightColor * (spec * material.specular);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}