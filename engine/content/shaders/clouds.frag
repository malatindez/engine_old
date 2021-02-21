#version 330 core
out vec4 FragColor;

#define MAX_TEXTURE_DIFFUSE_SIZE 8
#define MAX_TEXTURE_SPECULAR_SIZE 8 
struct Material {
    int diffuseSize;
    int specularSize;
    float shininess;
    sampler2D diffuseTextures[MAX_TEXTURE_DIFFUSE_SIZE];
    sampler2D specularTextures[MAX_TEXTURE_SPECULAR_SIZE];
}; 
in vec3 FragPos;
in vec3 Normal;  
in vec2 TexCoords;
in vec3 pos;

uniform vec3 viewPos;
uniform Material material;
uniform float stepF;
void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 fColor = vec3(0);
    vec2 newT = vec2(TexCoords.x + sin(stepF * 2 * 3.1415927f / 500000), TexCoords.y + cos((stepF+0.5) * 2 * 3.1415927f / 1250000));
    if(texture(material.diffuseTextures[0], newT).r <= 0.2) {
        discard;
    }
    FragColor = texture(material.diffuseTextures[0], newT).rgba - vec4(0.2,0.2,0.2,0);
}