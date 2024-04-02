#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <rg/Model.h>
#include <learnopengl/camera.h>
#include <rg/Hexagon.h>
#include <rg/Texture2D.h>

#include <iostream>
#include <vector>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 0.5f;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct DirLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 hexagonPosition = glm::vec3(0.0f, -11.0f, 0.0f);
    glm::vec3 ballerinaPosition = glm::vec3(0.1f, 0.0f, -0.5f);
    glm::vec3 butterflyPosition1 = glm::vec3(-6.0f, 1.0f, 0.0f);
    glm::vec3 butterflyPosition2 = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 windowPosition = glm::vec3(0.0f, 25.0f, 0.0f);
    float hexagonScale = 30.0f;
    float ballerinaScale = 20.0f;
    float butterflyScale = 0.2f;
    float teaCupScale = 0.08f;
    float flowerScale = 0.01f;
    float windowScale = 15.0f;
    DirLight dirLight;
    PointLight pointLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;
void setShaderUniformValues(rg::Shader& shader, DirLight& dirLight, PointLight& pointLight);
glm::mat4* getInstanceTransformationMatrices(unsigned int amount, float radius, float offset, float yoffset, float mscale);
void renderQuad();

std::vector<float> hexagonPositions {
        0.0f,  0.0f, 0.0f,     // center
        0.5f, 0.0f, 0.0f,      // middle right
        0.25f,  0.5f, 0.0f,    // top right
        -0.25f, 0.5f, 0.0f,    // top left
        -0.5f, 0.0f, 0.0f,     // middle left
        -0.25f,  -0.5f, 0.0f,  // bottom left
        0.25f,  -0.5f, 0.0f    // bottom right
};

std::vector<float> hexagonTextureCoord {
        0.5f, 0.5f,    // center
        1.0f, 0.5f,    // middle right
        0.75f, 1.0f,   // top right
        0.25f, 1.0f,   // top left
        0.0f, 0.5f,    // middle left
        0.25f, 0.0f,   // bottom left
        0.75f, 0.0f    // bottom right
};

int main() {
    // glfw initialize
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Trapped in a rabbit hole", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    rg::Shader hexagonShader("resources/shaders/HexagonShader.vs", "resources/shaders/HexagonShader.fs");
    rg::Shader modelShader("resources/shaders/ModelShader.vs", "resources/shaders/ModelShader.fs");
    rg::Shader teaCupShader("resources/shaders/InstanceModel.vs", "resources/shaders/InstanceModel.fs");
    rg::Shader flowerShader("resources/shaders/InstanceModel.vs", "resources/shaders/InstanceModel.fs");
    rg::Shader blendingShader("resources/shaders/BlendingShader.vs", "resources/shaders/BlendingShader.fs");
    rg::Shader hdrShader("resources/shaders/hdr.vs", "resources/shaders/hdr.fs");

    // load models
    rg::Model ballerina("resources/objects/ballerina_skeleton/scene.gltf");
    rg::Model butterfly("resources/objects/butterfly/scene.gltf");
    rg::Model teaCup("resources/objects/teaCup/scene.gltf");
    rg::Model flower("resources/objects/flower/scene.gltf");

    // hexagon
    rg::Texture2D hexagonDiffuseMap("resources/textures/stone.jpg");
    rg::Texture2D hexagonNormalMap("resources/textures/stoneNormal.jpg");
    rg::Texture2D hexagonHeightMap("resources/textures/stoneDisplacement.jpg");
    rg::Texture2D transparentTexture("resources/textures/stars.png");

    rg::Hexagon hexagon(hexagonPositions, hexagonTextureCoord, true);
    rg::Hexagon hexagonBlending(hexagonPositions, hexagonTextureCoord, false);

    // transformation matrices
    unsigned int amount = 20;
    glm::mat4* teaCupMatrices = getInstanceTransformationMatrices(amount, 18.0, 5.0, 30.0, programState->teaCupScale);
    unsigned int teaCupBuffer;
    glGenBuffers(1, &teaCupBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, teaCupBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &teaCupMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < teaCup.meshes.size(); i++)
    {
        unsigned int teaCupVAO = teaCup.meshes[i].VAO;
        glBindVertexArray(teaCupVAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    amount = 40;
    glm::mat4* flowerMatrices = getInstanceTransformationMatrices(amount, 20.0, 15.0, 40.0, programState->flowerScale);
    unsigned int flowerBuffer;
    glGenBuffers(1, &flowerBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, flowerBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &flowerMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < flower.meshes.size(); i++)
    {
        unsigned int flowerVAO = flower.meshes[i].VAO;
        glBindVertexArray(flowerVAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    // light
    DirLight& dirLight = programState->dirLight;
    dirLight.position = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    dirLight.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    dirLight.specular = glm::vec3(0.5f, 0.3f, 0.3f);

    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(0.0f, -10.0f, 0.0f);
    pointLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    pointLight.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.35f;
    pointLight.quadratic = 0.44f;

    // hdr
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // Render
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // hexagon
        hexagonShader.use();
        setShaderUniformValues(hexagonShader, dirLight, pointLight);
        hexagonShader.setVec3("lightPos", pointLight.position);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,programState->hexagonPosition);
        model = glm::scale(model, glm::vec3(programState->hexagonScale));
        model = glm::rotate(model, (float) glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        hexagonShader.setMat4("model", model);
        hexagonShader.setFloat("heightScale", 0.1f);

        hexagonShader.setInt("material.diffuseMap", 0);
        hexagonShader.setInt("material.normalMap", 1);
        hexagonShader.setInt("material.depthMap", 2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hexagonDiffuseMap.getId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, hexagonNormalMap.getId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, hexagonHeightMap.getId());

        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
        hexagon.drawHexagon();
        glCullFace(GL_FRONT);

        // ballerina
        modelShader.use();
        setShaderUniformValues(modelShader, dirLight, pointLight);
        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(programState->ballerinaScale));
        model = glm::rotate(model, (float) glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model,programState->ballerinaPosition);
        modelShader.setMat4("model", model);
        ballerina.Draw(modelShader);

        // butterfly
        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(0.8f * programState->butterflyScale));
        model = glm::rotate(model, (float) glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, glm::radians((float)glfwGetTime() * -20), glm::normalize(glm::vec3(0.0f, 0.0f, 0.3f)));
        model = glm::translate(model,programState->butterflyPosition1);
        modelShader.setMat4("model", model);
        butterfly.Draw(modelShader);

        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(programState->butterflyScale));
        model = glm::rotate(model, (float) glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * -20), glm::normalize(glm::vec3(0.2f, 0.5f, 0.5f)));
        model = glm::translate(model,programState->butterflyPosition2);
        modelShader.setMat4("model", model);
        butterfly.Draw(modelShader);

        // tea cup
        teaCupShader.use();
        setShaderUniformValues(teaCupShader, dirLight, pointLight);
        teaCupShader.setInt("material.diffuseMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, teaCup.loaded_textures[0].id);
        teaCupShader.setInt("material.specularMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, teaCup.loaded_textures[0].id);
        for (unsigned int i = 0; i < teaCup.meshes.size(); i++)
        {
            glBindVertexArray(teaCup.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, teaCup.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        // flower
        setShaderUniformValues(flowerShader, dirLight, pointLight);
        flowerShader.setInt("material.diffuseMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flower.loaded_textures[0].id);
        flowerShader.setInt("material.specularMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, flower.loaded_textures[0].id);
        for (unsigned int i = 0; i < flower.meshes.size(); i++)
        {
            glBindVertexArray(flower.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, flower.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        // blending
        blendingShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        blendingShader.setMat4("view", view);
        blendingShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->windowPosition);
        model = glm::scale(model, glm::vec3(programState->windowScale));
        model = glm::rotate(model, (float) glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        blendingShader.setMat4("model", model);

        blendingShader.setInt("texture1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transparentTexture.getId());
        hexagonBlending.drawHexagon();

        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // hdr
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        hdrShader.setInt("hdr", hdr);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    hexagon.free();
    hexagonBlending.free();
    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    glfwTerminate();
    return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions                     // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void setShaderUniformValues(rg::Shader& shader, DirLight& dirLight, PointLight& pointLight) {
    shader.setVec3("viewPos", programState->camera.Position);

    shader.setVec3("dirLight.direction", dirLight.position);
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);

    shader.setVec3("pointLight[0].position", pointLight.position);
    shader.setVec3("pointLight[0].ambient", pointLight.ambient);
    shader.setVec3("pointLight[0].diffuse", pointLight.diffuse);
    shader.setVec3("pointLight[0].specular", pointLight.specular);
    shader.setFloat("pointLight[0].constant", pointLight.constant);
    shader.setFloat("pointLight[0].linear", pointLight.linear);
    shader.setFloat("pointLight[0].quadratic", pointLight.quadratic);
    shader.setVec3("pointLight[0].color", glm::vec3(1.0f, 0.8f, 0.0f));

    shader.setVec3("pointLight[1].position", programState->butterflyPosition1);
    shader.setVec3("pointLight[1].ambient", pointLight.ambient);
    shader.setVec3("pointLight[1].diffuse", pointLight.diffuse);
    shader.setVec3("pointLight[1].specular", pointLight.specular);
    shader.setFloat("pointLight[1].constant", pointLight.constant);
    shader.setFloat("pointLight[1].linear", 0.14f);
    shader.setFloat("pointLight[1].quadratic", 0.07f);
    shader.setVec3("pointLight[1].color", glm::vec3(10.0f, 10.0f, 15.0f));

    shader.setVec3("pointLight[2].position", programState->butterflyPosition2);
    shader.setVec3("pointLight[2].ambient", pointLight.ambient);
    shader.setVec3("pointLight[2].diffuse", pointLight.diffuse);
    shader.setVec3("pointLight[2].specular", pointLight.specular);
    shader.setFloat("pointLight[2].constant", pointLight.constant);
    shader.setFloat("pointLight[2].linear", 0.7f);
    shader.setFloat("pointLight[2].quadratic", 1.8f);
    shader.setVec3("pointLight[2].color", glm::vec3(10.0f, 10.0f, 7.0f));

    shader.setFloat("material.shininess", 32.0f);

    glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = programState->camera.GetViewMatrix();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
}

glm::mat4* getInstanceTransformationMatrices(unsigned int amount, float radius, float offset, float yoffset, float mscale) {
    srand(glfwGetTime());
    glm::mat4* modelMatrices = new glm::mat4[amount];
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * yoffset * 100)) / 100.0f - yoffset;
        float y = displacement;
        if(y < -11)
            y += rand() % (int)(yoffset);
        if(y > 25)
            y -= rand() % (int)(yoffset);
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement * 0.4f;
        model = glm::translate(model, glm::vec3(x, y, z));

        model = glm::scale(model, glm::vec3(mscale));

        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }

    return modelMatrices;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed) {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        hdrKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        exposure += 0.001f;
    }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
