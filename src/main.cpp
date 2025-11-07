#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <iostream>
#include <cmath>

#include "shader.hpp"
#include "camera.hpp"

#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);

float mixValue = 0.2f;

const int width = 800;
const int height = 600;

Camera camera = glm::vec3(0.0f, 1.0f, 3.0f);
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

    glEnable(GL_DEPTH_TEST);

    if (GL_KHR_debug) 
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(openglDebugCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        std::cerr << "GL_KHR_debug не підтримується драйвером\n";
    }
//
    Shader lampShader ("shaders/vertex/lamp.vs",  "shaders/fragment/lamp.fs");
    Shader lightShader("shaders/vertex/light.vs", "shaders/fragment/light.fs");

    // Указывание вершин (и буферов) и настройка вершинных атрибутов
 
    // Добавляем новый набор вершин для формирования второго треугольника (всего 6 вершин)
    // float firstTriangle[] = 
    // {   
    //   // координаты    // текстурные координаты
    // -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //
    // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    // -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //
    // -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    // -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //
    //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //
    // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    // -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    // -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    // };

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // unsigned int indices[] = 
    // {
    //     0, 1, 3,  // трикутник 1
    //     1, 2, 3   // трикутник 2
    // };

    glm::vec3 cubePositions[] = 
    {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    unsigned int VBO, EBO; 
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightVAO;       
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Загрузка и создание текстур
    unsigned int texture1, texture2;
	
    // // Текстура №1 - Деревянный ящик
    // glGenTextures(1, &texture1);
    // glBindTexture(GL_TEXTURE_2D, texture1);
	//
    // // Установка параметров наложения текстуры
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
    // // Установка параметров фильтрации текстуры
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    // int width1, height1, nrChannels1;
    // const char* texturePath1 = "assets/textures/nogfy.jpeg";
    // stbi_set_flip_vertically_on_load(true);
    // unsigned char* data = stbi_load(texturePath1, &width1, &height1, &nrChannels1, 0);
    // if (data) 
    // {
    //     std::cout << "Texture1: " << width1 << "x" << height1 << " channels: " << nrChannels1 << "\n";
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load Texture2 from: " << texturePath1 << std::endl;
    //     std::cout << "Current path: " << std::filesystem::current_path() << "\n";
    // }
    // stbi_image_free(data);
    //
    // // Текстура №2 - Смайлик
    // glGenTextures(1, &texture2);
    // glBindTexture(GL_TEXTURE_2D, texture2);
	//
    // // Установка параметров наложения текстуры
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
    // // Установка параметров фильтрации текстуры
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    // int width2, height2, nrChannels2;
    // const char* texturePath2 = "assets/textures/awesomeface.png";
    // stbi_set_flip_vertically_on_load(true);
    // data = stbi_load(texturePath2, &width2, &height2, &nrChannels2, 0);
    // if (data)
    // {
    //     std::cout << "Texture2: " << width2 << "x" << height2 << " channels: " << nrChannels2 << "\n";
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load Texture2 from: " << texturePath2 << std::endl;
    //     std::cout << "Current path: " << std::filesystem::current_path() << "\n";
    // }
    // stbi_image_free(data);
    //
    // GLint loc1 = glGetUniformLocation(ourShader.ID, "texture1");
    // GLint loc2 = glGetUniformLocation(ourShader.ID, "texture2");
    // std::cout << "texture1 location: " << loc1 << " (should be >= 0)\n";
    // std::cout << "texture2 location: " << loc2 << " (should be >= 0)\n";

    lightShader.use();
    lightShader.setVec3("lightPos", lightPos);

    while (!glfwWindowShouldClose(window)) 
    {
        float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // Обработка ввода
        processInput(window);

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebuffer_size_callback(window, fbWidth, fbHeight);
        //
        // Рендеринг
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        lampShader.use();
        lampShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        float time = glfwGetTime();

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        lampShader.setMat4("model", model);

        // Рендеринг куба (lit object)
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Также отрисовываем наш объект-"лампочку" 
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // куб меньшего размера
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(VAO);
        // for (unsigned int i = 0; i < 10; i++) 
        // {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
		// 	// float angle = 20.0f * i;
		// 	// model = glm::rotate(model, glm::radians(angle * time), glm::vec3(1.0f, 0.3f, 0.5f));
        //     normShader.setMat4("model", model);
        //
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // glfw: обмен содержимым front- и back-буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    std::cout << "End \n";
    std::cout << glGetString(GL_VERSION) << std::endl;


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
    std::cerr << "GL DEBUG [" << id << "]: " << message << std::endl;
    std::cerr << "Type: " << type << ", Severity: " << severity << ", Source: " << source << std::endl;
}