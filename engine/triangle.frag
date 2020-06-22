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

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec3 pos;

uniform vec3 viewPos;
uniform Material material;

#define NR_DIRECT_LIGHTS 1
#define NR_SPOT_LIGHTS 1 
#define NR_POINT_LIGHTS 4

#if NR_DIRECT_LIGHTS != 0
uniform DirLight dirLights[NR_DIRECT_LIGHTS];
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // диффузное освещение
    float diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // комбинируем результаты
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularTextures[0], TexCoords));
    return (ambient + diffuse + specular);
}
#endif
#if NR_POINT_LIGHTS != 0
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // диффузное освещение
    float diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // затухание
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // комбинируем результаты
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularTextures[0], TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
#endif
#if NR_SPOT_LIGHTS != 0
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuseTextures[0], TexCoords).rgb;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseTextures[0], TexCoords).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specularTextures[0], TexCoords).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return (ambient + diffuse + specular);
} 
#endif
float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
uniform float stepx;
uniform float stepy;
uniform float stepz;
void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 fColor = vec3(0);
    FragColor = texture(material.diffuseTextures[0], TexCoords).rgba;
	return;
    for(int i = 0; i < NR_DIRECT_LIGHTS; i++) {
        fColor += CalcDirLight(dirLights[i], norm, viewDir);
    }
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        fColor += CalcPointLight(pointLights[i], norm, viewDir);
    }
#if NR_SPOT_LIGHTS != 0
    for(int i = 0; i < NR_DIRECT_LIGHTS; i++) {
        fColor += CalcSpotLight(spotLights[i], norm, viewDir);
    }
#endif
    
}