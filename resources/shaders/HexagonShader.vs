#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out VS_OUT {
    vec3 FragPos;
    vec2 TextureCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
uniform Light light;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TextureCoord = aTextureCoord;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * light.position;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}