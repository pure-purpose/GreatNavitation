#include <iostream>
#include <algorithm>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Shader.h>
#include <Camera.h>
#include <Model.h>

#include "TextRenderer.h"
#include "Voyage.h"

GLFWwindow *init();
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
unsigned int loadCubemap(std::vector<std::string> faces);
void getLightPos();

#define PI (acos(-1))
int ScreenWidth = 800;
int ScreenHeight = 600;
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

Camera camera;
bool DrawNormals = false;
bool Pause = false;

glm::vec3 shipPosition(-500.0f, -70.3f, 300.0f);
float shipForward = 0;
glm::vec3 shipForwardxz(cos(shipForward), 0, sin(shipForward));
float shipVelocity;
glm::vec3 relative(12, 50, -50);
float angle = 0;

glm::vec3 lightPos(0.0f, 0.5f, 0.0f);
float light_radius = 0.5f;
float light_angle = 90.0f * PI / 180;

glm::vec2 wind_direction(2.0f, 2.0f);
glm::vec2 wind_velocity(2.0f, 2.0f);

float skyboxVertices[] = {        
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

int main()
{
    GLFWwindow *window = init();
    if (window == nullptr) {
        std::cout << "Failed to initialize GLFW and OpenGL!" << std::endl;
        return -1;
    }

    Shader shader("./shaders/Water.vert", "./shaders/Water.frag");
    Shader textShader("./shaders/TextShader.vert", "./shaders/TextShader.frag");

    Model ship("./model/Fishing_boat/Boat.obj");
    Shader shipshader("./model/Fishing_boat/boat.vs", "./model/Fishing_boat/boat.fs");

    TextRenderer textRenderer;
    
    std::vector<std::string> faces
    {
        "./textures/skybox/right.jpg",
        "./textures/skybox/left.jpg",
        "./textures/skybox/top.jpg",
        "./textures/skybox/bottom.jpg",
        "./textures/skybox/front.jpg",
        "./textures/skybox/back.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    Shader skyboxShader("./shaders/skybox.vs", "./shaders/skybox.fs");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera.Position = glm::vec3(0.0f, 20.0f, 40.0f);

    Voyage ocean(wind_direction, wind_velocity, 128, 0.02f,1000);
    ocean.generateWave((float)glfwGetTime());
    unsigned int VBO, VBO2, EBO, VAO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.vertexCount, &ocean.vertices[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.normalCount, &ocean.normals[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ocean.indexCount, &ocean.indices[0], GL_STATIC_DRAW);
    
    unsigned int skyboxVAO, skyboxVBO;
    
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    short acount = 0;
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = (float)glfwGetTime();
        DeltaTime = currentFrame - LastFrame;
        LastFrame = currentFrame;
        auto currentFPS = (int)(1.0f / DeltaTime);
        shipVelocity = DeltaTime * camera.MovementSpeed * 80.0f * 4.0f;

        processInput(window);

        if (!Pause) {
            ocean.generateWave((float)glfwGetTime());
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.vertexCount, &ocean.vertices[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ocean.normalCount, &ocean.normals[0], GL_DYNAMIC_DRAW);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)ScreenWidth / ScreenHeight, 0.1f, 5000.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.Position);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        glm::vec3 ambient(0.2f, 0.2f, 0.2f);
        glm::vec3 diffuse(1.0f, 1.0f, 1.0f);
        glm::vec3 specular(1.0f, 1.0f, 1.0f);
        shader.setVec3("light.ambient", ambient);
        shader.setVec3("light.diffuse", diffuse);
        shader.setVec3("light.specular", specular);
        glm::vec3 basecolor(0.1f, 0.2f, 0.35f);
        shader.setVec3("basecolor", basecolor);

        glBindVertexArray(VAO);

        shader.setInt("skybox", 0);
        glm::mat4 omodel;
        glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                omodel = glm::mat4(1.0f);
                omodel = glm::rotate(omodel, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                omodel = glm::translate(omodel, glm::vec3(47.50f * (i-10), 0.0f, 47.50f * (j-10)));
                shader.setMat4("model", omodel);
                glDrawElements(GL_TRIANGLES, ocean.indexCount, GL_UNSIGNED_INT, nullptr);
            }
        }
        glBindVertexArray(0);


        shipshader.use();
        shipshader.setMat4("projection", projection);
        shipshader.setMat4("view", view);

        if (!acount)
        {
            if (angle > 0.01)
                angle -= (rand() / (double)RAND_MAX) / 20;
            else if (angle < -0.01)
                angle += (rand() / (double)RAND_MAX) / 20;
            else
                angle += (rand() / (double)RAND_MAX - 0.5f) / 10;
            acount = 3;
        }
        else
        {
            acount--;
        }
        shipPosition.y += rand()/(double)RAND_MAX  / 2 - 0.25f;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
        model = glm::translate(model, shipPosition);
        model = glm::rotate(model, shipForward, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, angle, shipForwardxz);
        shipshader.setMat4("model", model);
        ship.Draw(shipshader);
        

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//�Ƴ��任�����λ�Ʋ���
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        
        textRenderer.projection = glm::ortho(0.0f, (float)ScreenWidth,
                                             0.0f, (float)ScreenHeight);
        textRenderer.renderText(textShader, "FPS: " + std::to_string(currentFPS),
            340.0f, 260.0f, 0.3f,
            glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use WSAD to move, mouse to look around",
            0.0f, 289.0f, 0.3f,
            glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use Q to switch between polygon and fill mode",
            0.0f, 275.0f, 0.3f,
            glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use V to increase the wind speed",
            0.0f, 260.0f, 0.3f,
            glm::vec3(1.0, 0.0f, 0.0f));
        textRenderer.renderText(textShader, "Use B to decrease the wind speed",
            0.0f, 245.0f, 0.3f,
            glm::vec3(1.0, 0.0f, 0.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow *init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Great Navitation", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glViewport(0, 0, ScreenWidth * 2, ScreenHeight * 2);

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    ScreenWidth = width;
    ScreenHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // esc exit the program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WSAD control ship move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, DeltaTime);
        shipPosition += shipForwardxz * shipVelocity;

    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, DeltaTime);
        shipPosition -= shipForwardxz * shipVelocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, DeltaTime);
        shipForward += 0.0015 * 4.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, DeltaTime);
        shipForward -= 0.0015 * 4.0f;
    }
    shipForwardxz.x = -cos(shipForward);
    shipForwardxz.z = sin(shipForward);
    
    static bool isPolygon = false;
    static double lastPressedTime = 0.0;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        if (isPolygon) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            isPolygon = false;
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            isPolygon = true;
        }
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS
        && glfwGetTime() - lastPressedTime > 0.2) {
        Pause = !Pause;
        lastPressedTime = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        light_angle -= 2.0 / 90;
        if (light_angle < 15.0 * PI / 180) {
            light_angle = 15.0 * PI / 180;
        }
        getLightPos();
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        light_angle += 2.0 / 90;
        if (light_angle > 155.0 * PI / 180) {
            light_angle = 155.0 * PI / 180;
        }
        getLightPos();
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        wind_velocity += glm::vec2(0.1, 0.1);
        //cout << wind_velocity.x << " " << wind_velocity.y << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        //����û������
        if (wind_velocity.x > 0) {
            wind_velocity -= glm::vec2(0.1, 0.1);
        }
    }
    
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    static float lastMouseX = 400.0f;
    static float lastMouseY = 300.0f;
    static bool firstMouse = true;

    if (firstMouse) {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }

    float offsetX = (float)xpos - lastMouseX;
    float offsetY = (float)ypos - lastMouseY;
    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    camera.ProcessMouseMovement(offsetX, -offsetY);
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{   
    GLint last_texture_ID;
    GLuint textureID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    return textureID;
}

void getLightPos()
{
    float x = light_radius * cos(light_angle);
    float y = light_radius * sin(light_angle);
    lightPos = glm::vec3(x, y, 0.0);
}