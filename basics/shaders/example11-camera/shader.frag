#version 330 core
out vec4 FragColor;

// Our input vertex color received from our vertex shader (must be same name and same type)
in vec3 vertexColor;
in vec2 TexCoord;

// out vec3 vertexInputColor;
// uniform vec3 openglInputColor;

// NEW --- texture() is how you can create a texture
// NEW --- first parameter is the actual sampler
// NEW --- second parameter are going to be the texture coordinates
uniform sampler2D inputTexture1; // Texture that we give this fragment shader to render from OpenGL.

// Our second texture that we use to render.
uniform sampler2D inputTexture2;

void main(){
    // FragColor = vec4(vertexColor, 1.0);
    // FragColor = vec4(vertexInputColor, 1.0);
    // FragColor = vec4(openglInputColor, 1.0);

    // One thing to note is this is how you can load a texture through fragment shader
    // FragColor = texture(inputTexture, TexCoord);
    
    // Another option is if you want to modify the texture's color you can multiply that texture by the rgb value (vec4)
    // FragColor = texture(inputTexture, TexCoord) * vec4(1.0, 0.5, 0.5, 1.0);

    // NEW ---- How we can overlap two textures together
    // Takes in three parameters
    // First two parameters takes our textures
    // Third parameter takes the offset for linear interpolating the two textures together
    FragColor = mix(texture(inputTexture1, TexCoord), texture(inputTexture2, TexCoord), 0.2);
}