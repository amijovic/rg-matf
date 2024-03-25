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

void renderQuad();

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    glm::vec3 teaCupPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 violinPosition = glm::vec3(20.0f, -15.0f, 5.0f);
    glm::vec3 flowerPosition = glm::vec3(400.0f, 0.0f, 200.0f);
    float hexagonScale = 30.0f;
    float teaCupScale = 0.080f;
    float violinScale = 0.08f;
    float flowerScale = 0.01f;
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

void DrawImGui(ProgramState *programState);

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_BACK);
    //glEnable(GL_CW);
    // glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    rg::Shader hexagonShader("resources/shaders/HexagonShader.vs", "resources/shaders/HexagonShader.fs");
    rg::Shader teaCupShader("resources/shaders/TeaCup.vs", "resources/shaders/TeaCup.fs");
    rg::Shader violinShader("resources/shaders/Violin.vs", "resources/shaders/Violin.fs");
    rg::Shader flowerShader("resources/shaders/Flower.vs", "resources/shaders/Flower.fs");

    // load models
    rg::Model teaCup("resources/objects/teaCup/scene.gltf");
    rg::Model violin("resources/objects/violin/scene.gltf");
    rg::Model flower("resources/objects/flower/scene.gltf");

    // hexagon
    rg::Texture2D hexagonDiffuseMap("resources/textures/stone.jpg");
    rg::Texture2D hexagonNormalMap("resources/textures/stone_normal.jpg");
    rg::Texture2D hexagonHeightMap("resources/textures/stone_displacement.jpg");
    rg::Hexagon hexagon(hexagonPositions, hexagonTextureCoord);
    hexagonShader.use();
    hexagonShader.setInt("material.diffuseMap", 0);
    hexagonShader.setInt("material.normalMap", 1);
    hexagonShader.setInt("material.depthMap", 2);

    // light
    DirLight& dirLight = programState->dirLight;
    dirLight.position = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    dirLight.diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
    dirLight.specular = glm::vec3(0.5f, 0.3f, 0.3f);

    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(0.0f, -10.0f, 0.0f);
    pointLight.ambient = glm::vec3(0.4f, 0.4f, 0.4f);
    pointLight.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

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

        //pointLight.position = glm::vec3(-10.0 * cos(currentFrame), -10.0f, -10.0 * sin(currentFrame));
        dirLight.position = glm::vec3(-0.2f, -1.0f, -0.3f);
        pointLight.position = glm::vec3(0.0f, -10.0f, 0.0f);

        // hexagon
        hexagonShader.use();
        hexagonShader.setVec3("lightPos", pointLight.position);
        hexagonShader.setVec3("viewPos", programState->camera.Position);

        hexagonShader.setVec3("dirLight.direction", dirLight.position);
        hexagonShader.setVec3("dirLight.ambient", dirLight.ambient);
        hexagonShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        hexagonShader.setVec3("dirLight.specular", dirLight.specular);

        hexagonShader.setVec3("pointLight.position", pointLight.position);
        hexagonShader.setVec3("pointLight.ambient", pointLight.ambient);
        hexagonShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        hexagonShader.setVec3("pointLight.specular", pointLight.specular);

        hexagonShader.setFloat("pointLight.constant", pointLight.constant);
        hexagonShader.setFloat("pointLight.linear", pointLight.linear);
        hexagonShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        hexagonShader.setFloat("material.shininess", 32.0f);
        hexagonShader.setFloat("heightScale", 0.1f);

        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,programState->hexagonPosition);
        model = glm::scale(model, glm::vec3(programState->hexagonScale));
        model = glm::rotate(model, (float) glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
        //model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0))); // rotate the quad to show normal mapping from multiple directions

        hexagonShader.setMat4("model", model);
        hexagonShader.setMat4("view", view);
        hexagonShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hexagonDiffuseMap.getId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, hexagonNormalMap.getId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, hexagonHeightMap.getId());
        hexagon.drawHexagon();

        // tea cup
        teaCupShader.use();
        pointLight.position = glm::vec3(0.5, 1.0f, 0.3);

        teaCupShader.setVec3("dirLight.direction", dirLight.position);
        teaCupShader.setVec3("dirLight.ambient", dirLight.ambient);
        teaCupShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        teaCupShader.setVec3("dirLight.specular", dirLight.specular);

        teaCupShader.setVec3("pointLight.position", pointLight.position);
        teaCupShader.setVec3("pointLight.ambient", pointLight.ambient);
        teaCupShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        teaCupShader.setVec3("pointLight.specular", pointLight.specular);

        teaCupShader.setFloat("pointLight.constant", pointLight.constant);
        teaCupShader.setFloat("pointLight.linear", pointLight.linear);
        teaCupShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        teaCupShader.setVec3("viewPosition", programState->camera.Position);
        teaCupShader.setFloat("material.shininess", 32.0f);

        projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(programState->teaCupScale));
        model = glm::translate(model,programState->teaCupPosition);

        teaCupShader.setMat4("model", model);
        teaCupShader.setMat4("projection", projection);
        teaCupShader.setMat4("view", view);
        teaCup.Draw(teaCupShader);

        // violin
        violinShader.setVec3("dirLight.direction", dirLight.position);
        violinShader.setVec3("dirLight.ambient", dirLight.ambient);
        violinShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        violinShader.setVec3("dirLight.specular", dirLight.specular);

        violinShader.setVec3("pointLight.position", pointLight.position);
        violinShader.setVec3("pointLight.ambient", pointLight.ambient);
        violinShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        violinShader.setVec3("pointLight.specular", pointLight.specular);

        violinShader.setFloat("pointLight.constant", pointLight.constant);
        violinShader.setFloat("pointLight.linear", pointLight.linear);
        violinShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        violinShader.setVec3("viewPosition", programState->camera.Position);
        violinShader.setFloat("material.shininess", 32.0f);

        projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(programState->violinScale));
        model = glm::rotate(model, (float) glm::radians(3*90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model,programState->violinPosition);

        violinShader.setMat4("model", model);
        violinShader.setMat4("projection", projection);
        violinShader.setMat4("view", view);
        violin.Draw(violinShader);

        // flower
        flowerShader.setVec3("dirLight.direction", dirLight.position);
        flowerShader.setVec3("dirLight.ambient", dirLight.ambient);
        flowerShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        flowerShader.setVec3("dirLight.specular", dirLight.specular);

        flowerShader.setVec3("pointLight.position", pointLight.position);
        flowerShader.setVec3("pointLight.ambient", pointLight.ambient);
        flowerShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        flowerShader.setVec3("pointLight.specular", pointLight.specular);

        flowerShader.setFloat("pointLight.constant", pointLight.constant);
        flowerShader.setFloat("pointLight.linear", pointLight.linear);
        flowerShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        flowerShader.setVec3("viewPosition", programState->camera.Position);
        flowerShader.setFloat("material.shininess", 32.0f);

        projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        model = glm::mat4 (1.0f);
        model = glm::scale(model, glm::vec3(programState->flowerScale));
        model = glm::rotate(model, (float) glm::radians(3*90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model,programState->flowerPosition);

        flowerShader.setMat4("model", model);
        flowerShader.setMat4("projection", projection);
        flowerShader.setMat4("view", view);
        flower.Draw(flowerShader);

        if (programState->ImGuiEnabled)
            DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    hexagon.free();
    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        //ImGui::DragFloat3("Backpack position", (float*)&programState->backpackPosition);
        ImGui::DragFloat("Backpack scale", &programState->hexagonScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
