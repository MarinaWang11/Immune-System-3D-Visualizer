#version 330 core
out vec4 FragColor;

#define NR_SPOT_LIGHTS 4 

struct DirLight {
    vec3 direction;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

// uniform PointLight pointLights[NR_POINT_LIGHTS];  
uniform DirLight dirLight;
uniform SpotLight spotLights[NR_SPOT_LIGHTS]; 
uniform float shininess;
uniform vec3 viewPos; 
uniform vec3 objectColor;

uniform float alpha;

uniform bool clipEnabled;
uniform vec4 clipPlane;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // Propriedades normais e direção de visão
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);    
    
    if (clipEnabled){
        //distância do pixel até o plano de corte
        float distance = dot(FragPos, clipPlane.xyz) + clipPlane.w;
        
        if (distance < 0.0){
            discard; 
        }
    }
    
    FragColor = vec4(result * objectColor, alpha);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient  = light.ambient  * objectColor;
    vec3 diffuse  = light.diffuse  * diff * objectColor;
    vec3 specular = light.specular * spec * vec3(1.0f);
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Combine results
    vec3 ambient = light.ambient * objectColor;
    vec3 diffuse = light.diffuse * diff * objectColor;
    vec3 specular = light.specular * spec * vec3(1.0f);
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}

