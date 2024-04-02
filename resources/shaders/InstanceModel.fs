#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

#define POINT_LIGHT_NUMBER 3

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight dirLight;
uniform PointLight pointLight[POINT_LIGHT_NUMBER];
uniform Material material;

uniform vec3 viewPosition;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoords).rgb);
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 FragPos, vec3 viewDir) {
    // ambient
    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuseMap, TexCoords));

    // diffuse
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.color * pointLight.diffuse * diff * vec3(texture(material.diffuseMap, TexCoords).rgb);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * spec * vec3(texture(material.specularMap, TexCoords).rgb);

    // attenuation
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    vec3 result = calcDirLight(dirLight, normal, viewDir);
    for (int i = 0; i < POINT_LIGHT_NUMBER; i++)
        result += calcPointLight(pointLight[i], normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}