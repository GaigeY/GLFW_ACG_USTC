#include <cmath>
#include <iostream>
#include <cmath>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float centerX = SCR_WIDTH / 2.0f;
float centerY = SCR_HEIGHT / 2.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
double xpos, ypos;
float origin_x, origin_y;

bool lbutton_down = false;
bool rbutton_down = false;

float vertices[] = {
        -0.5f,  -0.5f,  0.0f, // left
        0.5f,   -0.5f,  0.0f, // right
        0.0f,   0.5f,   0.0f // top
};

const GLchar *vertexShaderSource = "#version 330 core\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
                                   "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

// mouse callback
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *, double xoffset, double yoffset);

// create callback function
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// process all input: exit when press "ESC".
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// Calculate con and sin between the two vectors.
float get_vector_cos(float x_1, float y_1, float x_2, float y_2);
float get_vector_sin(float x_1, float y_1, float x_2, float y_2);

int main() {
    // init the GLFW window.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // creat GLFW window.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EXP2 for ACG, USTC. Gaige Ying SA19229052.", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // init GLAD.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to Initialize GLAD" << std::endl;
        return -1;
    }

    // 定点着色器
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    // check shader compile errors.
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 片段着色器
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // check shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 着色器程序
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // vertex data & config

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float ), (void *)nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // a loop to keep window alive.
    while (!glfwWindowShouldClose(window)) {
        glfwSetKeyCallback(window, key_callback);
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // drawing command
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // redraw the triangle.
        glad_glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    float delta_x, delta_y;
    float vec_x, vec_y;
    float _sin, _cos;
    float buffer_x, buffer_y;

    // manage lock: press which button down.
    if (action == GLFW_PRESS) {
        switch (button) {

            case GLFW_MOUSE_BUTTON_LEFT: {
                if (!lbutton_down && !rbutton_down) {
                    lastX = float (xpos);
                    lastY = float (ypos);
                    lbutton_down = true;
                }
                break;
            }

            case GLFW_MOUSE_BUTTON_RIGHT: {
                if (!lbutton_down && !rbutton_down) {
                    lastX = float (xpos);
                    lastY = float (ypos);
                    origin_x = lastX - centerX;
                    origin_y = lastY - centerY;
                    rbutton_down = true;
                }
                break;
            }
            default:
                return;
        }
    }

    // respond when release mouse button.
    if (action == GLFW_RELEASE) {
        switch (button) {

            case GLFW_MOUSE_BUTTON_LEFT: {
                if (lbutton_down) {
                    delta_x = float (xpos - lastX) / SCR_WIDTH;
                    delta_y = float (lastY - ypos) / SCR_HEIGHT;
                    lastX = float (xpos);
                    lastY = float (ypos);

                    for (int i = 0; i < 3; ++i) {
                        vertices[3 * i] += delta_x;
                        vertices[3 * i + 1] += delta_y;
                    }
                }
                lbutton_down = false;
                break;
            }

            case GLFW_MOUSE_BUTTON_RIGHT: {
                if (rbutton_down) {
                    vec_x = float(xpos) - centerX;
                    vec_y = float(ypos) - centerY;

                    _cos = get_vector_cos(origin_x, origin_y, vec_x, vec_y);
                    _sin = -get_vector_sin(origin_x, origin_y, vec_x, vec_y);

                    for (int i = 0; i < 3; ++i) {
                        buffer_x = vertices[3 * i] * centerX;
                        buffer_y = vertices[3 * i + 1] * centerY;

                        vertices[3 * i] = (buffer_x * _cos - buffer_y * _sin) / centerX;
                        vertices[3 * i + 1] = (buffer_x * _sin + buffer_y * _cos) / centerY;
                    }
                }
                rbutton_down = false;
                break;
            }
            default:
                break;
        }
    }

}

// catch scroll signal to scale the graph.
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    for (float & vertice : vertices) {
        vertice *= (1 + 0.01f * (float )yoffset);
    }
}

float get_vector_cos(float x_1, float y_1, float x_2, float y_2) {
    double _cos;
    _cos = (x_1*x_2 + y_1*y_2)/(std::sqrt(x_1*x_1 + y_1*y_1) * std::sqrt(x_2*x_2 + y_2*y_2));
    return float (_cos);
}
float get_vector_sin(float x_1, float y_1, float x_2, float y_2) {
    float normal_x = -y_1;
    float normal_y = x_1;
    return get_vector_cos(normal_x, normal_y, x_2, y_2);
}