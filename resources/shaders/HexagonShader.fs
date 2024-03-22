#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D depthMap;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TextureCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

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

    // ambient
    vec3 ambient = light.ambient * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}