#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* TITLE = "Rendering Window";
const int WIDTH = 800;
const int HEIGHT = 800;

const float SIZE = 100.0f;
const float SPEED = 0.15f;
GLfloat vertices[] = {
    0.0f, 0.0f, 0.0f, // top left      0
    SIZE, 0.0f, 0.0f, // top right     1
    0.0f, SIZE, 0.0f, // bottom left   2
    SIZE, SIZE, 0.0f  // bottom right  3
};

GLuint indices[] = {
    0, 1, 3,
    0, 2, 3
};

std::string ParseShaderFile(const char* path)
{
    // reads the file
    std::ifstream file(path, std::ios::in);
    if (!file) return "file commit die o.o";

    // outputs the file to stringf
    std::string currLine;
    std::stringstream content;
    while (getline(file, currLine))
    {
        content << currLine << '\n';
    }

    file.close();
    return content.str();
}

GLuint CreateShader(const char* path, GLuint shaderType)
{
    // gets script from shader file
    std::string shaderSrc = ParseShaderFile(path);
    const char* srcString = shaderSrc.c_str();

    // links script with shader
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &srcString, nullptr);
    glCompileShader(shader);

    // error checking
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) // error found
    {
        int len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        // sending error log
        char* msg = new char[len];
        glGetShaderInfoLog(shader, len, &len, msg);
        std::cout << "[ERROR]: " << msg << '\n';
        delete[] msg;
    }
    return shader;
}

GLuint CreateShaderProgram(GLuint &vertexShader, GLuint &fragmentShader)
{
    // linking shaders into one program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); 
    glValidateProgram(shaderProgram);

    // clears unused memory
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

bool CollisionCheck(glm::vec2 obj1, glm::vec2 obj2)
{
    if (obj1[0] < obj2[0] + SIZE &&
        obj1[0] + SIZE >  obj2.x &&
        obj1[1] < obj2[1] + SIZE &&
        obj1[1] + SIZE >   obj2[1])
        return true;

    return false;
}

void HandleMovement(GLFWwindow* window, glm::vec2& pos1, glm::vec2 pos2, GLuint up, GLuint down, GLuint left, GLuint right)
{
    if (glfwGetKey(window, up))
    {
        pos1[1] -= SPEED;
        if (/*CollisionCheck(pos1, pos2) ||*/ pos1[1] < 0)
            pos1[1] += SPEED;
    }
    if (glfwGetKey(window, down))
    {
        pos1[1] += SPEED;
        if (/*CollisionCheck(pos1, pos2) ||*/ pos1[1] + SIZE > HEIGHT)
            pos1[1] -= SPEED;
    }
    if (glfwGetKey(window, left))
    {
        pos1[0] -= SPEED;
        if (/*CollisionCheck(pos1, pos2) ||*/ pos1[0] < 0)
            pos1[0] += SPEED;
    }
    if (glfwGetKey(window, right))
    {
        pos1[0] += SPEED;
        if (/*CollisionCheck(pos1, pos2) ||*/ pos1[0] + SIZE > WIDTH)
            pos1[0] -= SPEED;
    }
}

int main(void)
{
    if (!glfwInit())
        return -1;

    // OpenGL version: 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creates window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, WIDTH, HEIGHT);

    // creates shader
    GLuint vertexShader = CreateShader("resources/shaders/vertex.shader", GL_VERTEX_SHADER);
    GLuint fragmentShader = CreateShader("resources/shaders/fragment.shader", GL_FRAGMENT_SHADER);
    GLuint shaderProgram = CreateShaderProgram(vertexShader, fragmentShader);

    // creates object
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // binds object
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // creates attributes for object
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // creates order in which points are aligned
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // unbinds everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // mvp
    glm::vec2 pos1 = glm::vec2(0.0f);
    glm::vec2 pos2 = glm::vec2(0.0f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::ortho(0.0f, (float) WIDTH, (float) HEIGHT, 0.0f, -1.0f, 1.0f);

    // locations
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "u_mvp");
    GLuint collidingLoc = glGetUniformLocation(shaderProgram, "u_colliding");

    glClearColor(0.0f, 0.15f, 0.25f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        // creates first object
        HandleMovement(window, pos1, pos2, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos1, 1.0f));
        glm::mat4 mvp1 = proj * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp1));
        glUniform1i(collidingLoc, CollisionCheck(pos1, pos2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // creates second object
        HandleMovement(window, pos2, pos1, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(pos2, 1.0f));
        glm::mat4 mvp2 = proj * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp2));
        glUniform1i(collidingLoc, CollisionCheck(pos1, pos2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glfwTerminate();
    return 0;
}