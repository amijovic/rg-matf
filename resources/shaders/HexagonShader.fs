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
    sampler2D normalMap;
    sampler2D depthMap;
    float shininess;
};

#define POINT_LIGHT_NUMBER 3

in VS_OUT {
    vec3 FragPos;
    vec2 TextureCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight[POINT_LIGHT_NUMBER];

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height = texture(material.depthMap, texCoords).r;
    return texCoords - viewDir.xy * (height * heightScale);
}

void main() {
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TextureCoord;

    texCoords = ParallaxMapping(fs_in.TextureCoord, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    vec3 normal = texture(material.normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 color = texture(material.diffuseMap, fs_in.TextureCoord).rgb;

    // directional light
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 reflectDir = reflect(-lightDir, normal);
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = dirLight.ambient * color;
    vec3 diffuse = dirLight.diffuse * diff * color;
    vec3 specular = dirLight.specular * spec * color;
    vec3 result = ambient + diffuse + specular;

    for (int i = 0; i < POINT_LIGHT_NUMBER; i++) {
        // point light
        // ambient
        ambient = pointLight[i].ambient * color;

        // diffuse
        lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        diff = max(dot(lightDir, normal), 0.0);
        diffuse = pointLight[i].color * pointLight[i].diffuse * diff * color;

        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        specular = pointLight[i].specular * spec * color;

        // attenuation
        float distance = length(pointLight[i].position - fs_in.FragPos);
        float attenuation = 1.0 / (pointLight[i].constant + pointLight[i].linear * distance + pointLight[i].quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        result = result + ambient + diffuse + specular;
    }
    FragColor = vec4(result, 1.0);
}