#version 330 core
out vec4 FragColor;

// Specifying our materials properties
// Specifying lighting (3 ways) and shininess of this material
// Removing ambient material color vector since ambient is normally equal to diffuse colors anyways, as we've seen.
// Now controlling ambient with the light, so no need to store ambient.
// NEW --- Defining material without ambient and diffuse
struct Material{
    sampler2D diffuse; // NEW -- Repplacing ambient and diffuse with our texture
    // vec3 specular;
    sampler2D specular;
    float shininess;
};

// Specfying our Light Component
struct Light{
    // vec3 position; // NEW -- Not needed when using directional lightings
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// NEW --- CREATING OUR POINT LIGHTS
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[4];

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// NEW --- Setting this through OpenGL!
uniform DirectLight directLight;

uniform vec3 viewPos;
// Our material set through opengl code
uniform Material material;
uniform Light light;

// NEW --- Computing our direct lighting instead of calculating this in the main function!
vec3 CalculateDirectLighting(DirectLight light, vec3 normal, vec3 viewDirection){
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectionDirection = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    // Combining to output
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

// NEW --- Computing the point lights that we create!
vec3 CalculatePointLighting(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection){
    vec3 lightDirection = normalize(light.position - fragPos);

    // Diffuse Shading
    float diff = max(dot(normal, lightDirection), 0.0);

    // Specular Shading
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Getting our computation result
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

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
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    // // Diffused Lighting
    // vec3 normal = normalize(Normal);
    // // vec3 lightDirection = normalize(light.position - FragPos);
    // vec3 lightDirection = normalize(-light.direction); // NEW ---- Getting our direction of our direction light sources

    // float diff = max(dot(normal, lightDirection), 0.0);
    // // NEW ---- Setting our material.diffuse map texture to our diffused lighting
    // // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // // specular lighting
    // vec3 viewDirection = normalize(viewPos - FragPos);
    // vec3 reflectionDirection = reflect(-lightDirection, normal);
    // // float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    // float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    // // vec3 specular = lightColor * (spec * material.specular);
    // // Setting our material.diffuse map texture to our diffused lighting for our specular lighting
    // // vec3 specular = light.specular * (spec * material.specular);
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));


    // vec3 result = (ambient + diffuse + specular) * objectColor;
    // vec3 result = ambient + diffuse + specular;


    // NEW --- Now setting our properties
    vec3 norm = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);

    // NEW ---- PHASE #1 - Directional Lighting
    vec3 result = CalculateDirectLighting(directLight, norm, viewDirection);

    // NEW ---- PHASE #2 - Point Lighting
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalculatePointLighting(pointLights[i], norm, FragPos, viewDirection);
    }
    
    FragColor = vec4(result, 1.0);
}