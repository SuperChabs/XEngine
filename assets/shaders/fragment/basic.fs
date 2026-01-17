#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform bool useColor;

struct Material 
{
    vec3 color;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

struct Light 
{
    int type; 
    
    vec3 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
    
    float innerCutoff;
    float outerCutoff;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;

vec3 CalcDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(-light.direction);
    
    vec3 ambient = light.ambient * objectColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectColor;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    vec3 ambient = light.ambient * objectColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectColor;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    vec3 ambient = light.ambient * objectColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectColor;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    diffuse *= intensity;
    specular *= intensity;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 color;
    if (useColor)
    {
        color = material.color;
    }
    else
    {
        vec4 texColor = texture(material.texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
            discard;
        color = texColor.rgb;
    }
    
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < numLights; i++)
    {
        if (lights[i].type == 0) // DIRECTIONAL
            result += CalcDirectionalLight(lights[i], norm, viewDir, color);
        else if (lights[i].type == 1) // POINT
            result += CalcPointLight(lights[i], norm, FragPos, viewDir, color);
        else if (lights[i].type == 2) // SPOT
            result += CalcSpotLight(lights[i], norm, FragPos, viewDir, color);
    }
    
    if (numLights == 0)
    {
        vec3 ambient = vec3(0.3);
        result = color * ambient;
    }
    
    FragColor = vec4(result, 1.0);
}