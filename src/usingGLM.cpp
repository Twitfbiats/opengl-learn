#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.cpp>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <getPath.cpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "Width: " << width << " Height: " << height << std::endl;
}  

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    
    float vertices[] = {
     // positions         // colors         //texture cords
    -1.f, -1.f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  
    1.f, -1.f, 0.0f,  0.0f, 1.0f, 0.0f,  .0f, 1.0f,   
    -1.f,  1.f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, .0f,
    1.f,  1.f, 0.0f,  0.3f, 0.3f, .3f  , .0f, .0f 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        1, 2, 3    // second triangle
    };  

    std::string projectPath = getPath(argc, argv);
    std::string vertexPath = projectPath + "\\\\shader\\\\glm_vertex.glsl";
    std::string fragmentPath = projectPath + "\\\\shader\\\\glm_fragment.glsl";
    Shader testShader(vertexPath.data(), fragmentPath.data());
    
    // jpg path is src/container.jpg - now create a string path
    std::string jpgPath = projectPath + "\\\\img\\\\container.jpg";
    int width, height, nrChannels;
    unsigned char *data = stbi_load(jpgPath.data(), &width, &height, &nrChannels, 0); 
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1); 
    glBindTexture(GL_TEXTURE_2D, texture1);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    {
        //stbi_image_free(data);
        jpgPath = projectPath + "\\\\img\\\\container1.jpg";
        data = stbi_load(jpgPath.data(), &width, &height, &nrChannels, 0);
        glGenTextures(1, &texture2); 
        glBindTexture(GL_TEXTURE_2D, texture2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    testShader.use();
    testShader.setInt("texture1", 0);
    testShader.setInt("texture2", 1);

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // for getting the maximum number of vertex attributes
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    glm::mat4 rotMat;
    glm::vec4 vec;
    float time;
    
    while(!glfwWindowShouldClose(window))
    {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        time = fmod(glfwGetTime(), 360.) / 1000;
        {
            vec = glm::vec4(vertices[0], vertices[1], vertices[2], 1.0f);
            rotMat = glm::mat4(1.0f);
            rotMat = glm::rotate(rotMat, time, glm::vec3(0.0f, 1.0f, 0.0f));
            vec = rotMat * vec;
            vertices[0] = vec.x; vertices[1] = vec.y; vertices[2] = vec.z;

            vec = glm::vec4(vertices[8], vertices[9], vertices[10], 1.0f);
            rotMat = glm::mat4(1.0f);
            rotMat = glm::rotate(rotMat, time, glm::vec3(0.0f, 1.0f, 0.0f));
            vec = rotMat * vec;
            vertices[8] = vec.x; vertices[9] = vec.y; vertices[10] = vec.z;

            vec = glm::vec4(vertices[16], vertices[17], vertices[18], 1.0f);
            rotMat = glm::mat4(1.0f);
            rotMat = glm::rotate(rotMat, time, glm::vec3(0.0f, 1.0f, 0.0f));
            vec = rotMat * vec;
            vertices[16] = vec.x; vertices[17] = vec.y; vertices[18] = vec.z;

            vec = glm::vec4(vertices[24], vertices[25], vertices[26], 1.0f);
            rotMat = glm::mat4(1.0f);
            rotMat = glm::rotate(rotMat, time, glm::vec3(0.0f, 1.0f, 0.0f));
            vec = rotMat * vec;
            vertices[24] = vec.x; vertices[25] = vec.y; vertices[26] = vec.z;

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        }
        testShader.use();
        glBindVertexArray(VAO);
        
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW); 

        testShader.setFloat("time", glfwGetTime()/3.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();   
        processInput(window); 
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
  
    return 0;
}