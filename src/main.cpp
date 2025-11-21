#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <vector>
#include <filesystem>
#include <map>

#include "core/shader.hpp"
#include "core/camera.hpp"
#include "scene/model.hpp"

//#define STBI_NO_FAILURE_STRINGS
#include <stb_image.h>

using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(std::vector<std::string> faces);

float mixValue = 0.2f;

const int width = 800;
const int height = 600;

Camera camera = vec3(0.0f, 0.0f, 3.0f);
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: инициализация и конфигурирование
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello!", NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    else
        std::cout << "Successful created GLFW window\n";

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    else
        std::cout << "Successful initialize GLAD\n";

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);

    Shader shader("assets/shaders/vertex/depth.vs", "assets/shaders/fragment/depth.fs"); 
    Shader normalShader("assets/shaders/vertex/normalDisplay.vs", "assets/shaders/fragment/depth.fs", "assets/shaders/geometry/normalDisplay.gs");
    Shader skyboxShader("assets/shaders/vertex/skybox.vs", "assets/shaders/fragment/skybox.fs");

    float vCubeTextures[] = 
    {
        // Задняя грань
        0.0f, 0.0f, // нижняя-левая
        1.0f, 0.0f, // нижняя-правая    
        1.0f, 1.0f, // верхняя-правая              
        1.0f, 1.0f, // верхняя-правая
        0.0f, 1.0f, // верхняя-левая
        0.0f, 0.0f, // нижняя-левая   
                
        // Передняя грань
        0.0f, 0.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-правая        
        1.0f, 1.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-левая
        0.0f, 1.0f, // верхняя-левая  
        
        // Грань слева
        1.0f, 0.0f, // верхняя-правая
        0.0f, 1.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-левая       
        0.0f, 1.0f, // нижняя-левая
        1.0f, 0.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-правая
    
        // Грань справа
        1.0f, 0.0f, // верхняя-левая
        1.0f, 1.0f, // верхняя-правая      
        0.0f, 1.0f, // нижняя-правая          
        0.0f, 1.0f, // нижняя-правая
        0.0f, 0.0f, // нижняя-левая
        1.0f, 0.0f, // верхняя-левая
    
        // Нижняя грань          
        0.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-левая
        1.0f, 1.0f, // верхняя-левая        
        1.0f, 0.0f, // нижняя-левая
        0.0f, 1.0f, // верхняя-правая
        0.0f, 0.0f, // нижняя-правая
    
        // Верхняя грань
        0.0f, 1.0f, // верхняя-левая
        1.0f, 1.0f, // верхняя-правая
        1.0f, 0.0f, // нижняя-правая                 
        1.0f, 0.0f, // нижняя-правая
        0.0f, 0.0f, // нижняя-левая  
        0.0f, 1.0f  // верхняя-левая  
    };

    float vCubeVertexes[] = 
    {
        // Задняя грань
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // Передняя грань
        -0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,

        // Грань слева
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f,

        // Грань справа
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        // Нижняя грань 
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,

        // Верхняя грань
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    float vCubeNormales[] = 
    {
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f, -1.0f, 
    
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
    
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f, 
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
    
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
    
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
    
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    float planeVertices[] = 
    {
         // координаты        // текстурные координаты 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    float quadVertices[] = 
    { 
         // координаты // текстурные координаты
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    float skyboxVertices[] = 
    {
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

    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes) + sizeof(vCubeNormales) + sizeof(vCubeTextures), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vCubeVertexes), &vCubeVertexes); //вершины
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes), sizeof(vCubeNormales), &vCubeNormales); //нормали
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vCubeVertexes) + sizeof(vCubeNormales), sizeof(vCubeTextures), &vCubeTextures); // текстуры

    // VAO куба
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(vCubeVertexes)+sizeof(vCubeNormales)));
    glBindVertexArray(0);
	
    // VAO пола
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

	// VAO скайбокса
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Загрузка текстур
    unsigned int cubeTexture  = loadTexture("assets/textures/wooden_container.png");
    unsigned int floorTexture = loadTexture("assets/textures/metal.jpg");

    std::vector<std::string> faces = 
    {
        "assets/textures/skybox2/right.png",
        "assets/textures/skybox2/left.png",
        "assets/textures/skybox2/top.png",
        "assets/textures/skybox2/bottom.png",
        "assets/textures/skybox2/front.png",
        "assets/textures/skybox2/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // Настройка шейдера
    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0); 

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model nanosuit("assets/objects/backpack/backpack.obj");

    while (!glfwWindowShouldClose(window)) 
    {
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebuffer_size_callback(window, fbWidth, fbHeight);

        // Обработка ввода
        processInput(window);

        // glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);        

        // Рендеринг
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4 model = mat4(1.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 projection = perspective(radians(camera.GetZoom()), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setMat4("model", model);

        nanosuit.Draw(shader);

        // Куб
        // glBindVertexArray(cubeVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // model = translate(model, vec3(-1.0f, 0.01f, -1.0f));
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // Пол
        // glBindVertexArray(planeVAO);
        // glBindTexture(GL_TEXTURE_2D, floorTexture);
        // shader.setMat4("model", mat4(1.0f));
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

        // SkyBox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        mat4 skyboxView = mat4(mat3(view));
        skyboxShader.setMat4("view", skyboxView);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glEnable(GL_DEPTH_TEST);

        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "End \n";
    std::cout << glGetString(GL_VERSION) << std::endl;

    // glDeleteVertexArrays(1, &cubeVAO); 
    // glDeleteVertexArrays(1, &cubeMirVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    // glDeleteVertexArrays(1, &planeVAO);
    // glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVBO);
    // glDeleteBuffers(1, &planeVBO);

    // glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse) 
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
    camera.ProcessScrool(yoffset);
}

void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam)
{
    std::cerr << "GL DEBUG [" << id << "]: " << message << std::endl; //
    std::cerr << "Type: " << type << ", Severity: " << severity << ", Source: " << source << std::endl;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        GLenum tex;
        if (nrComponents == 1)
        {    
            format = GL_RED;
            tex = GL_REPEAT;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
            tex = GL_REPEAT;
        }   
        else if (nrComponents == 4) 
        {
            format = GL_RGBA;
            tex = GL_CLAMP_TO_EDGE;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) 
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        } 
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
